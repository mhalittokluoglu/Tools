
class LogLevels:
    def __init__(self):
        self.__Load_Dict()

    def __Load_Dict(self):
        lines = []
        with open("LogLevels.csv", 'r') as log_level_file:
            for each_line in log_level_file:
                each_line = each_line.replace('\n', '')
                lines.append(each_line)


        self.dictionary = {}

        for each_line in lines:
            cells = each_line.split(';')
            log_id = int(cells[0])
            log_string = cells[1]
            self.dictionary[log_id] = log_string

