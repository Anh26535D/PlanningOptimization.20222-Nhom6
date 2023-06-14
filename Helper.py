def read_input(file_path):
    '''Read input data from file_path for container 2d loading problem.

        Parameters
        ----------
        file_path : str
            Path to file containing the input data

        Returns
        -------
        n_items : int
            Number of items.

        n_trucks : int
            Number of trucks.

        items : list 
            List of tuple (item's width, item's height).
            
        trucks : list
            List of tuple (truck's width, truck's height, truck's cost).
    '''
    with open(file_path) as f:
        data = f.read().splitlines()
        n_items, n_trucks = map(int, data[0].split())
        items = [tuple(map(int, line.split())) for line in data[1:n_items+1]]
        trucks = [tuple(map(int, line.split())) for line in data[n_items+1:]]

    return n_items, n_trucks, items, trucks