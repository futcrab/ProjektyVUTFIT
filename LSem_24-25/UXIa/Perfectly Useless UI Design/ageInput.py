import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel, QCheckBox
from PyQt5.QtGui import QFont
from PyQt5.QtCore import Qt
from random import randint

class MyApp(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.age = 0
        self.last_die = 0

        layout = QVBoxLayout()
        self.label = QLabel(f"Your age is: {self.age} Roll the dice !", self)
        self.label.setFont(QFont("Comic Sans MS", 14))
        self.label.setAlignment(Qt.AlignCenter)

        self.roll = QPushButton("Roll the dice !", self)
        self.roll.setFont(QFont("Comic Sans MS", 11))
        self.roll.setFixedWidth(200)
        
        self.button = QPushButton("Thats my age !", self)
        self.button.setFont(QFont("Comic Sans MS", 11))
        self.button.setFixedWidth(200)
        
        self.roll.clicked.connect(self.roll_dice)
        self.button.clicked.connect(self.end_program)

        self.button_layout = QHBoxLayout()
        self.button_layout.addWidget(self.roll)
        self.button_layout.addWidget(self.button)
        self.button_layout.setAlignment(Qt.AlignCenter)
        
        self.add_checkbox = QCheckBox("Add", self)
        self.add_checkbox.setFont(QFont("Comic Sans MS", 11))
        self.sub_checkbox = QCheckBox("Subtract", self)
        self.sub_checkbox.setFont(QFont("Comic Sans MS", 11))

        self.add_checkbox.setChecked(True)
        self.add_checkbox.stateChanged.connect(self.toggle_checkboxes)
        self.sub_checkbox.stateChanged.connect(self.toggle_checkboxes)
        
        checkbox_layout = QHBoxLayout()
        checkbox_layout.addWidget(self.add_checkbox)
        checkbox_layout.addWidget(self.sub_checkbox)
        checkbox_layout.setAlignment(Qt.AlignCenter)
    
        layout.addLayout(checkbox_layout)
        layout.addWidget(self.label)
        layout.addLayout(self.button_layout)
        
        self.setLayout(layout)
        self.setWindowTitle('Worst Age Input')
        self.show()

    def toggle_checkboxes(self):
        sender = self.sender()
        if sender.isChecked():
            if sender == self.add_checkbox:
                self.sub_checkbox.setChecked(False)
            else:
                self.add_checkbox.setChecked(False)

    def end_program(self):
        if self.age < 0:
            self.label.setStyleSheet("color: red;")
            self.label.setText("You are not born yet ! Continue rolling the dice.")
        elif self.age < 18:
            self.label.setStyleSheet("color: red;")
            self.label.setText("You are a minor ! Continue rolling the dice.")
        elif self.age > 99:
            self.label.setStyleSheet("color: red;")
            self.label.setText("You are too old ! Continue rolling the dice.")
        else:
            QApplication.instance().quit()

    def roll_dice(self):
        die = randint(1, 6)

        if self.last_die == 6 and die == 6:
            self.label.setStyleSheet("color: green;")
            self.age = self.age + 2000
            self.label.setText(f"Extra bonus ! You rolled two 6's ! Your age is now {self.age}")
            self.last_die = 0
            return

        if self.last_die == 1 and die == 1:
            self.label.setStyleSheet("color: red;")
            self.label.setText(f"Bad Luck ! You rolled two 1's ! You can never attempt to roll the dice again !")
            self.roll.setDisabled(True)
            self.button.setDisabled(True)
            return

        if self.add_checkbox.isChecked():
            self.age = self.age + die
        else:
            self.age = self.age - die
        self.label.setStyleSheet("color: black;")
        self.label.setText(f"Die rolled {die}, Your age is: {self.age}")

        self.last_die = die

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = MyApp()
    sys.exit(app.exec_())