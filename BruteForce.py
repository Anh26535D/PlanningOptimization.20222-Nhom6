from Helper import read_input

class BruteForceSolver():

    def __init__(self, n_items, n_trucks, items, trucks):
        self.n_items = n_items
        self.n_trucks = n_trucks
        self.items = items
        self.trucks = trucks

        self.f = 0 
        self.f_best = 1e9

        self.t = [-1]*self.n_items
        self.x = [-1]*self.n_items
        self.y = [-1]*self.n_items
        self.o = [-1]*self.n_items
        self.t_solution = [-1]*self.n_items
        self.x_solution = [-1]*self.n_items
        self.y_solution = [-1]*self.n_items
        self.o_solution = [-1]*self.n_items

        self.prior_trucks = list(range(self.n_trucks))
        self.prior_trucks.sort(key=lambda k: self.trucks[k][0] * self.trucks[k][1], reverse=True)
    
    def printSolution(self, opt=False):
        if opt:
            for i in range(self.n_items):
                print(i+1, self.t_solution[i]+1, self.x_solution[i], self.y_solution[i], self.o_solution[i])
        else:
            for i in range(self.n_items):
                print(i+1, self.t[i]+1, self.x[i], self.y[i], self.o[i])   

    def isOverlapping(self, x1, y1, w1, h1, x2, y2, w2, h2):
        return max(0,min(x1 + w1, x2 + w2) - max(x1, x2)) * max(0, min(y1 + h1, y2 + h2) - max(y1, y2)) > 0

    def check(self, n, k, xn, yn, on):
        w, l = self.items[n][0], self.items[n][1]
        W, L = self.trucks[k][0], self.trucks[k][1]
        if(on==1):
            w, l = l, w

        if((0<=xn) and (xn+w <= W) and (0<=yn) and (yn+l <= L)):
            for i in range(n):
                if(self.t[i] == k):
                    wi, li = self.items[i][0], self.items[i][1]
                    xi, yi = self.x[i], self.y[i]
                    if(self.o[i] == 1):
                        wi, li = li, wi

                    if(self.isOverlapping(xi, yi, wi, li, xn, yn, w, l)):
                        return False
            return True
        return False
    
    def update(self, n, k, xn, yn, on):
        self.t[n] = k
        self.x[n] = xn
        self.y[n] = yn
        self.o[n] = on

    def restore(self, n):
        self.t[n] = -1
        self.x[n] = -1
        self.y[n] = -1
        self.o[n] = -1

    def calCurrentCost(self, n):
        f=0
        inTruck = [0]*self.n_trucks
        for i in range(n):
            inTruck[self.t[i]] += 1
        for i in range(self.n_trucks):
            if(inTruck[i] > 0):
                f += self.trucks[i][2]
        return f
    
    def solve(self, n):
        for tr in range(self.n_trucks):
            k = self.prior_trucks[tr]; 
            m = min(self.items[n][0], self.items[n][1])
            for xn in range(self.trucks[k][0] - m):
                for yn in range(self.trucks[k][1] - m):
                    for on in range(2):
                        if(self.check(n, k, xn, yn, on)):
                            self.update(n, k, xn, yn, on)
                            if(n == self.n_items-1):
                                f = self.calCurrentCost(n)
                                if(f < self.f_best):
                                    self.f_best = f
                                    for i in range(self.n_items):
                                        self.t_solution[i] = self.t[i]
                                        self.x_solution[i] = self.x[i]
                                        self.y_solution[i] = self.y[i]
                                        self.o_solution[i] = self.o[i]
                            else:
                                f = self.calCurrentCost(n)
                                if(f<self.f_best):
                                    self.solve(n+1)
                            self.restore(n)
    
    def writeToFile(self, file_path):
        with open(file_path, "w") as f:
            for i in range(self.n_items):
                f.writelines(f"{i+1} {self.t_solution[i]+1} {self.x_solution[i]} {self.y_solution[i]} {self.o_solution[i]}\n")

if __name__ == "__main__":
    input_path = "E:/planning_optimization/container2Dloading/data.in"
    output_path = "E:/planning_optimization/container2Dloading/data.out"

    n_items, n_trucks, items, trucks = read_input(file_path=input_path)

    solver = BruteForceSolver(n_items, n_trucks, items, trucks)
    solver.solve(0)
    solver.printSolution(opt=True)
    solver.writeToFile(output_path)