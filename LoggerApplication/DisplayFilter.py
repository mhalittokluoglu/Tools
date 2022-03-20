class DisplayFilter:
    def __init__(self):
        self.log_level_set = set()
    
    def Update_Filter(self, log_level_set):
        self.log_level_set = log_level_set

    def IsValidForFilter(self, log_message):
        if log_message.level in self.log_level_set:
            return True
        return False


