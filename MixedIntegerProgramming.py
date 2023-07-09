from ortools.linear_solver import pywraplp
from Helper import read_input
import os
cur_path = os.getcwd()

class MixedIntegerProgramming:

    def __init__(self, n_items, n_trucks, items, trucks, time_limit=None):
        self.n_items = n_items
        self.n_trucks = n_trucks
        self.items = items
        self.trucks = trucks

        self.time_limit = time_limit

        self.max_truck_width = max(x[0] for x in trucks)
        self.max_truck_height = max(x[1] for x in trucks)

        self.t_solution = [-1]*self.n_items
        self.x_solution = [-1]*self.n_items
        self.y_solution = [-1]*self.n_items
        self.o_solution = [-1]*self.n_items

    def setSolver(self):
        self.solver = pywraplp.Solver.CreateSolver('SCIP')

        # T[i, j] means item-i is placed in truck-j
        self.T = {}
        for i in range(n_items):
            for j in range(n_trucks):
                self.T[i, j] = self.solver.IntVar(0, 1, f'T_{i}_{j}')

        # R[i] means is item-i rotated
        self.R = [self.solver.IntVar(0, 1, f'R_{i}') for i in range(self.n_items)]

        # left coordination of item
        self.l = [self.solver.IntVar(0, self.max_truck_width, f'l_{i}') for i in range(self.n_items)]
        # right coordination of item
        self.r = [self.solver.IntVar(0, self.max_truck_width, f'r_{i}') for i in range(self.n_items)]
        # top coordination of item
        self.t = [self.solver.IntVar(0, self.max_truck_height, f't_{i}') for i in range(self.n_items)]
        # bottom coodination of item
        self.b = [self.solver.IntVar(0, self.max_truck_height, f'b_{i}') for i in range(self.n_items)]

        # Z[j] means truck-j is used
        self.Z = [self.solver.IntVar(0, 1, f'Z_{i}') for i in range(self.n_trucks)]

    def setConstraint(self):
        # Each item can only be placed in one truck
        for i in range(self.n_items):
            self.solver.Add(sum(self.T[i, j] for j in range(self.n_trucks)) == 1)

        # Item cannot be placed out of truck
        for i in range(self.n_items):      
            self.solver.Add(self.r[i] == (1-self.R[i]) * items[i][0] + self.R[i] * items[i][1] + self.l[i])
            self.solver.Add(self.t[i] == (1-self.R[i]) * items[i][1] + self.R[i] * items[i][0] + self.b[i])
            for k in range(self.n_trucks):
                self.solver.Add(self.r[i] <= (1-self.T[i, k]) * self.max_truck_width + self.trucks[k][0])
                self.solver.Add(self.l[i] <= (1-self.T[i, k]) * self.max_truck_width + self.trucks[k][0])
                self.solver.Add(self.t[i] <= (1-self.T[i, k]) * self.max_truck_height + self.trucks[k][1])
                self.solver.Add(self.b[i] <= (1-self.T[i, k]) * self.max_truck_height + self.trucks[k][1])  

        # Overlap condition
        for i in range(self.n_items-1):
            for j in range(i+1, self.n_items):
                for k in range(self.n_trucks):
                    self.c1 = self.solver.IntVar(0, 1, f'c1_{i}_{j}')
                    self.c2 = self.solver.IntVar(0, 1, f'c2_{i}_{j}')
                    self.c3 = self.solver.IntVar(0, 1, f'c3_{i}_{j}')
                    self.c4 = self.solver.IntVar(0, 1, f'c4_{i}_{j}')
                    
                    self.solver.Add(self.r[i] <= self.l[j] + self.max_truck_width * (1 - self.c1))
                    self.solver.Add(self.r[j] <= self.l[i] + self.max_truck_width * (1 - self.c2))
                    self.solver.Add(self.t[i] <= self.b[j] + self.max_truck_height * (1 - self.c3))
                    self.solver.Add(self.t[j] <= self.b[i] + self.max_truck_height * (1 - self.c4))

                    self.solver.Add(self.c1 + self.c2 + self.c3 + self.c4 >= self.T[i,k] + self.T[j,k] - 1)
                    self.solver.Add(self.c1 + self.c2 + self.c3 + self.c4 <= 4*self.T[j,k])
                    self.solver.Add(self.c1 + self.c2 + self.c3 + self.c4 <= 4*self.T[i,k])

        # Find which truck has been used 
        for k in range(self.n_trucks):
            self.solver.Add(self.Z[k] <= sum(self.T[i,k] for i in range(self.n_items)))
            self.solver.Add(sum(self.T[i,k] for i in range(self.n_items)) <= self.Z[k] * self.n_items)

    def setObjective(self):
        cost = sum(self.Z[k]*self.trucks[k][2] for k in range(self.n_trucks))
        self.solver.Minimize(cost)

    def solve(self):
        self.setSolver()
        self.setConstraint()
        self.setObjective()

        if self.time_limit is not None:
            self.solver.set_time_limit(self.time_limit * 1000)

        status = self.solver.Solve()
        if status == pywraplp.Solver.OPTIMAL or status == pywraplp.Solver.FEASIBLE:
            for i in range(self.n_items):
                for j in range(self.n_trucks):
                    if self.T[i,j].solution_value() == 1:
                        self.t_solution[i] = j
                        self.x_solution[i] = int(self.l[i].solution_value())
                        self.y_solution[i] = int(self.b[i].solution_value())
                        self.o_solution[i] = int(self.R[i].solution_value())

    def printSolution(self):
        for i in range(self.n_items):
            print(i+1, self.t_solution[i]+1, self.x_solution[i], self.y_solution[i], self.o_solution[i])

    def writeToFile(self, file_path):
        with open(file_path, "w") as f:
            for i in range(self.n_items):
                f.writelines(f"{i+1} {self.t_solution[i]+1} {self.x_solution[i]} {self.y_solution[i]} {self.o_solution[i]}\n")
    
if __name__ == '__main__':
    input_path = cur_path + "/INPUT.txt"
    output_path = cur_path + "/OUTPUT.txt"

    n_items, n_trucks, items, trucks = read_input(input_path)
    solver = MixedIntegerProgramming(n_items, n_trucks, items, trucks)
    solver.solve()
    solver.printSolution()
    # solver.writeToFile(output_path)
