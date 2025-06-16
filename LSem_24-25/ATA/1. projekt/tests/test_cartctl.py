#!/usr/bin/env python3
"""
Example of usage/test of Cart controller implementation.
"""

import sys
import unittest
from cartctl.cartctl import CartCtl, Status as CartCtlStatus
from cartctl.cart import Cart, CargoReq, Status
from cartctl.jarvisenv import Jarvis

def log(msg):
    "simple logging"
    print('  %s' % msg)

class TestCartRequests(unittest.TestCase):
    
    # In my tests Jarvis is a factory for making robots, thats why they are transferring body parts.
    def test_normal_mode(self):
        
        "Normal mode test"
        "F- 1,2,8,9"
        "P- 1,3"

        unload_times = [0, 0]

        def add_load(c: CartCtl, cargo_req: CargoReq):
            "callback for schedulled load"
            log('%d: Requesting %s at %s' % \
                (Jarvis.time(), cargo_req, cargo_req.src))
            if cargo_req.content == 'left_leg':
                self.assertEqual(Jarvis.time(), 10) # P-01
            if cargo_req.content == 'right_leg':
                self.assertEqual(Jarvis.time(), 31) # P-01
                self.assertEqual(c.cart.status, Status.MOVING) #F-01
            c.request(cargo_req)

        def on_move(c: Cart):
            if Jarvis.time() == 32: # P-03
                self.assertEqual(c.pos, 'B')
                self.assertEqual(c.data, 'A')
            
            log('%d: Cart is moving %s->%s' % (Jarvis.time(), c.pos, c.data))

        def on_load(c: Cart, cargo_req: CargoReq):
            self.assertLessEqual(Jarvis.time() - cargo_req.born, 60) # F-02
            log('%d: Cart at %s: loading: %s' % (Jarvis.time(), c.pos, cargo_req))
            log(c)
            cargo_req.context = "loaded"

        def on_unload(c: Cart, cargo_req: CargoReq):
            log('%d: Cart at %s: unloading: %s' % (Jarvis.time(), c.pos, cargo_req))
            log(c)
            self.assertEqual('loaded', cargo_req.context)
            cargo_req.context = 'unloaded'

            if cargo_req.content == 'left_leg':
                unload_times[0] = Jarvis.time()
            if cargo_req.content == 'right_leg':
                unload_times[1] = Jarvis.time()

        def test_position(cart: Cart, desired_pos: str):
            self.assertEqual(cart.pos, desired_pos)

        # Setup Cart and Cart Controller
        cart_dev = Cart(2, 150, 0)
        cart_dev.onmove = on_move
        c = CartCtl(cart_dev, Jarvis)

        # Setup Cargo to move
        left_leg = CargoReq('A', 'D', 20, 'left_leg')
        left_leg.onload = on_load
        left_leg.onunload = on_unload

        right_leg = CargoReq('A', 'D', 20, 'right_leg')
        right_leg.onload = on_load
        right_leg.onunload = on_unload

        # Setup Plan
        Jarvis.reset_scheduler()

        Jarvis.plan(10, add_load, (c,left_leg))
        Jarvis.plan(31, add_load, (c,right_leg))

        Jarvis.plan(10, test_position, (cart_dev, "A")) # F-08
        Jarvis.plan(33, test_position, (cart_dev, "B")) # F-08
        Jarvis.plan(63, test_position, (cart_dev, "A")) # F-08

        Jarvis.run()

        # Verify indirect output
        log(cart_dev)
        self.assertTrue(cart_dev.empty())
        self.assertEqual('unloaded', left_leg.context)
        self.assertEqual('unloaded', right_leg.context)

        self.assertLessEqual(unload_times[0] - unload_times[1], 2) # F-09

    def test_unload_only_mode(self):
        "Unloading only mode test"
        
        "F- 3,4,5,6,7,10"
        "P- 2,4,5"

        torso_load_time = 0

        def add_load(c: CartCtl, cargo_req: CargoReq):
            "callback for schedulled load"
            log('%d: Requesting %s at %s' % \
                (Jarvis.time(), cargo_req, cargo_req.src))
            c.request(cargo_req)

        def on_move(c: Cart):
            "example callback (for assert)"
            # put some asserts here
            log('%d: Cart is moving %s->%s' % (Jarvis.time(), c.pos, c.data))

        def on_load(c: Cart, cargo_req: CargoReq):
            "example callback for logging"
            log('%d: Cart at %s: loading: %s' % (Jarvis.time(), c.pos, cargo_req))
            log(c)
            cargo_req.context = "loaded"
            if cargo_req.content == 'torso':
                torso_load_time = Jarvis.time()


        def on_unload(c: Cart, cargo_req: CargoReq):
            "example callback (for assert)"
            # put some asserts here
            log('%d: Cart at %s: unloading: %s' % (Jarvis.time(), c.pos, cargo_req))
            log(c)
            self.assertEqual('loaded', cargo_req.context)
            cargo_req.context = 'unloaded'
        
        def check_cargo_status(cargo_req: CargoReq, desired_status):
            "check if cargo is unloaded, loaded, or waiting"
            self.assertEqual(cargo_req.context, desired_status)
        
        def check_cargo_priority(cargo_req: CargoReq, desired_prio: bool):
            "check if cargo is in prio mode"
            self.assertEqual(cargo_req.prio, desired_prio)

        def check_controller_mode(c: CartCtl, desired_status: CartCtlStatus):
            "check if controller is in unload only mode"
            self.assertEqual(c.status, desired_status)

        # Setup Cart and Cart Controller
        cart_dev = Cart(2, 150, 0)
        cart_dev.onmove = on_move
        c = CartCtl(cart_dev, Jarvis)

        # Setup Cargo to move
        left_leg = CargoReq('A', 'D', 20, 'left_leg')
        left_leg.onload = on_load
        left_leg.onunload = on_unload

        right_leg = CargoReq('A', 'D', 20, 'right_leg')
        right_leg.onload = on_load
        right_leg.onunload = on_unload

        torso = CargoReq('C', 'B', 50, 'torso')
        torso.onload = on_load
        torso.onunload = on_unload

        head = CargoReq('A', 'C', 15, 'head')
        head.onload = on_load
        head.onunload = on_unload

        # Setup Plan
        Jarvis.reset_scheduler()

        Jarvis.plan(10, add_load, (c,left_leg))
        Jarvis.plan(25, add_load, (c,right_leg))
        Jarvis.plan(70, add_load, (c,torso))
        Jarvis.plan(205, add_load, (c,head))

        Jarvis.plan(140, check_cargo_priority, (torso, True))  # F-03
        Jarvis.plan(200, check_cargo_status, (torso, 'loaded'))  # F-04
        Jarvis.plan(181, check_controller_mode, (c, CartCtlStatus.UNLOAD_ONLY))  # F-05, P-04
        Jarvis.plan(215, check_cargo_status, (head, None)) # F-06
        Jarvis.plan(233, check_controller_mode, (c, CartCtlStatus.NORMAL)) # F-07, P-05
        Jarvis.plan(110, check_cargo_status, (torso, None)) # F-10

        self.assertLessEqual(torso_load_time - torso.born, 120) # P-02

        Jarvis.run()

        # Verify indirect output
        log(cart_dev)
        self.assertTrue(cart_dev.empty())
        self.assertEqual('unloaded', left_leg.context)
        self.assertEqual('unloaded', right_leg.context)
        self.assertEqual('unloaded', torso.context)
        self.assertEqual('unloaded', head.context)

    def test_operational(self):
        "Operational test"
        "C- 5,6"
        
        def add_load(c: CartCtl, cargo_req: CargoReq):
            "callback for schedulled load"
            log('%d: Requesting %s at %s' % \
                (Jarvis.time(), cargo_req, cargo_req.src))
            c.request(cargo_req)

        def on_move(c: Cart):
            "example callback (for assert)"
            # put some asserts here
            log('%d: Cart is moving %s->%s' % (Jarvis.time(), c.pos, c.data))

        def on_load(c: Cart, cargo_req: CargoReq):
            "example callback for logging"
            log('%d: Cart at %s: loading: %s' % (Jarvis.time(), c.pos, cargo_req))
            log(c)
            cargo_req.context = "loaded"

        def on_unload(c: Cart, cargo_req: CargoReq):
            "example callback (for assert)"
            # put some asserts here
            log('%d: Cart at %s: unloading: %s' % (Jarvis.time(), c.pos, cargo_req))
            log(c)
            self.assertEqual('loaded', cargo_req.context)
            cargo_req.context = 'unloaded'

        # Setup Cart and Cart Controller
        cart_dev = Cart(1, 150, 0)
        cart_dev.onmove = on_move
        c = CartCtl(cart_dev, Jarvis)

        # Setup Cargo to move
        left_leg = CargoReq('A', 'D', 200, 'left_leg')
        left_leg.onload = on_load
        left_leg.onunload = on_unload

        right_leg = CargoReq('A', 'D', 20, 'right_leg')
        right_leg.onload = on_load
        right_leg.onunload = on_unload

        # Setup Plan
        Jarvis.reset_scheduler()
        Jarvis.plan(10, add_load, (c,left_leg))
        Jarvis.plan(25, add_load, (c,right_leg))

        Jarvis.run()

        # Verify indirect output
        log(cart_dev)
        self.assertTrue(cart_dev.empty())
        self.assertEqual(None, left_leg.context) # C-05
        self.assertEqual('unloaded', right_leg.context) # C-06

if __name__ == "__main__":
    unittest.main()
