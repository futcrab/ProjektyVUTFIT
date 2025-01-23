#include "search-strategies.h"
#include "memusage.h"
#include <set>
#include <deque>
#include <queue>

struct State{
	SearchState state;
	std::vector<SearchAction> actions;
	std::vector<SearchAction> path; // Added when implementing BFS, its not used in DFS
};

struct stateInfoAstar {
    std::shared_ptr<SearchState> state;
    std::vector<std::shared_ptr<SearchAction>> actions;
    double priority;

    bool operator>(const stateInfoAstar &other) const {
        return priority > other.priority;
    }
    stateInfoAstar() : state(nullptr), actions(), priority(0.0) {}

    stateInfoAstar(std::shared_ptr<SearchState> s, std::vector<std::shared_ptr<SearchAction>> a, double p)
        : state(std::move(s)), actions(std::move(a)), priority(p) {}
};

std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {
	std::vector<SearchAction> solution;
	std::vector<State> open_states;
	std::set<SearchState> closed_states;

	// insert first state into open_states
	open_states.push_back({SearchState(init_state), {}, {}});
	open_states.back().actions = open_states.back().state.actions();

	bool solution_found = false;
	bool no_solution = false;
	size_t usage = 0;

	// while there are states to explore
	while(!solution_found && !no_solution){
		// for each action in the current state
		for(int i = 0; i < open_states.front().actions.size(); i++){
			auto next_action = open_states.front().actions[i];
			auto next_state = next_action.execute(open_states.front().state);
			// if the state is already in closed_states, skip it
			if (closed_states.find(next_state) != closed_states.end()) 
				continue;
			
			// if the state is final, we found a solution
			if(next_state.isFinal()){
				solution_found = true;
				// solution is made of the path to the last state + next action
				solution = open_states.front().path;
				solution.push_back(next_action);
				break;
			}

			// if not in closed states and not final add the new state to open_states
			open_states.push_back({next_action.execute(open_states.front().state), {}, open_states.front().path});
			open_states.back().actions = open_states.back().state.actions();
			open_states.back().path.push_back(next_action);

			// check memory
			usage = getCurrentRSS();
			if (usage >= (mem_limit_ - 1000000)){
				printf("Memory limit reached\n");
				no_solution = true;
				break;
			}
		}
		
		// if we found a solution or there are no more states to explore, break
		if(solution_found || no_solution)
			break;

		// add the current state to closed_states
		closed_states.insert(open_states.front().state);
		open_states.erase(open_states.begin());

		// if there are no more states to explore, there is no solution
		if (open_states.size() == 0)
			no_solution = true;
	}

	// clear the vectors
	open_states.clear();
	closed_states.clear();

	// if there is no solution, return an empty vector
	if (no_solution)
		return std::vector<SearchAction>();
	return solution;
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {
	std::vector<SearchAction> solution;
	std::vector<State> last_states;
	
	// insert first state into last_states
	last_states.push_back({SearchState(init_state), {}, {}});
	last_states.back().actions = last_states.back().state.actions();

	int depth = 0;
	bool solution_found = false;
	bool no_solution = false;
	size_t usage = 0;
	
	// while there are states to explore
	while(!solution_found && !no_solution){

		// if the state is final, we found a solution
		if(last_states.back().state.isFinal()) {
			solution_found = true;
			break;
		}

		// if there are no more actions in the current state or we reached the depth limit
		if(last_states.back().actions.size() == 0 || depth == depth_limit_) {
			
			// if there are more states to explore, pop the last state and go back to the previous state and remove the last action
			if (last_states.size() > 1) {
				last_states.pop_back();
				solution.pop_back();

				depth--;
				continue;
			// if there are no more states to explore, there is no solution
			}else{
				no_solution = true;
				continue;
			}
		}

		// execute the next action
		auto next_action = last_states.back().actions.back();
		// remove the action from the list of actions so its not done again
		last_states.back().actions.pop_back();
		// add the action to the solution
		solution.push_back(next_action);

		// add the new state to last_states
		last_states.push_back({next_action.execute(last_states.back().state), {}, {}});
		last_states.back().actions = last_states.back().state.actions();
		depth++;

		// check memory
		usage = getCurrentRSS();
		if (usage >= (mem_limit_ - 1000000)){
			printf("Memory limit reached\n");
			no_solution = true;
			break;
		}
	}
	// clear the vectors
	last_states.clear();
	// if there is no solution, return an empty vector
	if (no_solution)
		return std::vector<SearchAction>();
	return solution;
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    return 0;
}

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
    std::set<SearchState> closed_states;
    std::priority_queue<stateInfoAstar, std::vector<stateInfoAstar>, std::greater<stateInfoAstar>> states_open;

    // init the starting state
    double cost = 0.0;
    stateInfoAstar initial_state = {std::make_shared<SearchState>(init_state), {}, cost};
    states_open.push(std::move(initial_state));

    // if the initial state is already the goal
    if (states_open.top().state->isFinal())
        return {};

    while (!states_open.empty()) {
        // get the state with the lowest cost
        auto curr_node = states_open.top();
        states_open.pop();

        auto curr_state = curr_node.state;
        auto curr_actions = curr_node.actions;

        // check if the goal state is reached
        if (curr_state->isFinal()) {
            std::vector<SearchAction> result;
            for (const auto &final_action : curr_actions)
                result.push_back(*final_action);
            return result;
        }

        // skip if the state has already been processed
        if (closed_states.find(*curr_state) != closed_states.end())
            continue;

        // mark the state as visited
        closed_states.insert(*curr_state);

        // expand the current state by applying all possible actions
        std::vector<SearchAction> availableActions = curr_state->actions();
        for (const auto &action : availableActions) {
            // check memory
            if ((getCurrentRSS() + 1000000) >= mem_limit_) {
				printf("Memory limit reached\n");
                return {};
            }

            // generate the next state by executing the action
            auto next_state = std::make_shared<SearchState>(action.execute(*curr_state));

            // skip if the next state has already been visited
            if (closed_states.find(*next_state) != closed_states.end())
                continue;

            // calculate the cost and heuristic for the next state
            double heuristic = compute_heuristic(*next_state, *heuristic_);
            double cost_total = curr_actions.size() + 1 + heuristic;

            // update the action path
            std::vector<std::shared_ptr<SearchAction>> next_actions = curr_actions;
            next_actions.push_back(std::make_shared<SearchAction>(action));

            // add the new state to the open list
            states_open.push({next_state, std::move(next_actions), cost_total});
        }
    }
    return {};
}