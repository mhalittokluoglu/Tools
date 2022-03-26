import tkinter as tk
from tkinter import font
from DisplayFilter import DisplayFilter
from LogLevelLoader import *
from UdpServer import * 
import threading

class LogMessage:
    def __init__(self):
        self.level = -1
        self.data = ''

class LoggerApplication:
    def __init__(self, win):
        self.win = win
        frame = tk.Frame(win, bg = '#303030')
        frame.rowconfigure(2, weight = 1)
        frame.columnconfigure(0, weight = 1)

        self.LogMessages = []
        self.FilteredLogMessages = []
        self.loglevels = LogLevels()
        self.ip_address = '127.0.0.1'
        self.port = 30000
        self.filters = DisplayFilter()
        self.__SetInitialFilters()
        self.checkbox_frame = tk.Frame(frame, padx = 15, bg = '#303030')
        ip_frame = tk.Frame(frame, bg = '#303030')
        command_frame = tk.Frame(frame, bg = '#303030', padx = 20)
        self.text_frame = tk.Frame(frame, bg = '#454545')
        self.__CreateFilterCheckBoxes()

        self.logger_frame = frame
        self.log_text = tk.Text(ip_frame, font = "Times 12")
        self.save_button = tk.Button(command_frame, text = "Save", foreground = '#DFDFDF', background = '#404040', font = 'Times 12')
        self.clear_logButton = tk.Button(command_frame, text = "Clear", foreground = '#DFDFDF', background = '#404040', font = 'Times 12', command = self.ClearAllText)

        self.ip_address_label = tk.Label(ip_frame, text = "Ip Address:", font = "Times 12", fg = "#DFDFDF", bg = "#303030")
        self.ip_address_entry = tk.Entry(ip_frame, font = "Times 12", fg = "#DFDFDF", bg = "#505050", disabledforeground = '#707070')
        self.port_address_label = tk.Label(ip_frame, text = "Port:", font = "Times 12", fg = "#DFDFDF", bg = "#303030")
        self.port_address_entry = tk.Entry(ip_frame, font = "Times 12", fg = "#DFDFDF", bg = "#505050", disabledforeground = '#707070')
        self.start_listener_button = tk.Button(ip_frame, text = 'Start Listening', foreground = '#DFDFDF', background = '#404040', command = self.StartUdpServer)

        self.ip_address_entry.insert(tk.END, "127.0.0.1")
        self.port_address_entry.insert(tk.END, "30000")

        self.ip_address_entry.bind('<Return>', self.IpAddressChanged)
        self.port_address_entry.bind('<Return>', self.PortChanged)


        self.text = tk.Text(self.text_frame, bg = '#505050', fg = '#DFDFDF', font = 'Times 12',
        highlightbackground = '#242424', highlightcolor= '#FDFDFD')

        scroller = tk.Scrollbar(self.text_frame, command = self.text.yview)
        self.text['yscrollcommand'] = scroller.set

        command_frame.grid(row = 1, column = 1, sticky = tk.N)
        self.clear_logButton.grid(row = 0, column = 0)
        self.save_button.grid(row = 0, column = 1)

        self.logger_frame.grid(row = 0, column = 0, sticky = tk.N + tk.W + tk.E + tk.S)
        self.ip_address_label.grid(row = 0, column = 0, sticky = tk.N + tk.W)
        self.ip_address_entry.grid(row = 0, column = 1, sticky = tk.N + tk.W)
        self.port_address_label.grid(row = 1, column = 0, sticky = tk.N + tk.W)
        self.port_address_entry.grid(row = 1, column = 1, sticky = tk.N + tk.W)
        self.start_listener_button.grid(row = 3, column = 1, columnspan = 2)

        self.checkbox_frame.grid(row = 0, column = 1, columnspan= len(self.check_boxes), sticky = tk.N)
        ip_frame.grid(row = 0, column = 0)
        self.text_frame.grid(row = 2, column = 0, columnspan = 2, sticky= tk.N + tk.E + tk.S + tk.W)
        self.text_frame.rowconfigure(0, weight = 1)
        self.text_frame.columnconfigure(0, weight = 1)
        self.text.grid(row = 0, column = 0, sticky= tk.N + tk.E + tk.S + tk.W)

        scroller.grid(row = 0, column = 1, sticky = tk.N + tk.S)

        self.server_running = False
        self.win.protocol(
            'WM_DELETE_WINDOW', self.__ApplicationClosed)


    def __ApplicationClosed(self):
        if self.server_running:
            self.server.Close()
        self.win.destroy()

    def __SetInitialFilters(self):
        for log_id in self.loglevels.dictionary:
            self.filters.log_level_set.add(log_id)

    def __CreateFilterCheckBoxes(self):
        self.check_boxes = []
        self.check_box_vars = []
        checkbox_label = tk.Label(self.checkbox_frame, text = 'Filters', font = 'Times 12', bg = '#404040', fg = '#DFDFDF')
        for level_id in self.loglevels.dictionary:
            level_str = self.loglevels.dictionary[level_id]
            var = tk.BooleanVar()
            check_box = tk.Checkbutton(self.checkbox_frame, text = level_str, variable = var, 
                    onvalue = True, offvalue= False, command = self.CheckBoxChanged,
                    bg = '#454545', fg= '#ECECEC', selectcolor = '#303030', font = 'Times 12')
            self.check_box_vars.append(var)
            check_box.select()
            
            self.check_boxes.append(check_box)

        checkbox_label.grid(row = 0)
        for i in range(0, len(self.check_boxes)):
            self.check_boxes[i].grid(row = 1, column = i)

    
    def CheckBoxChanged(self):
        self.filters.log_level_set.clear()
        for i in range(0, len(self.check_box_vars)):
            if self.check_box_vars[i].get():
                self.filters.log_level_set.add(i)
        self.UpdateDisplayMessages()


    def IpAddressChanged(self, e):
        self.ip_address = self.ip_address_entry.get()

    def PortChanged(self, e):
        self.port = int(self.port_address_entry.get())

    def StartUdpServer(self):
        self.server_running = True
        self.server = UdpServer(self.ip_address, self.port)
        self.server.SetCallBack(self.OnMessageReceived)
        self.start_listener_button['text'] = 'Stop Listening'
        self.start_listener_button['command'] = self.StopUdpServer
        self.ip_address_entry['state'] = tk.DISABLED
        self.port_address_entry['state'] = tk.DISABLED
        self.listenerThread = threading.Thread(target = self.server.ReceiveData)
        self.listenerThread.start()

    def StopUdpServer(self):
        self.server_running = False
        self.server.Close()
        self.start_listener_button['text'] = 'Start Listening'
        self.start_listener_button['command'] = self.StartUdpServer
        self.ip_address_entry['state'] = tk.NORMAL
        self.port_address_entry['state'] = tk.NORMAL
        

    def OnMessageReceived(self, data):
        data_messages = data.split('\n')
        logMessages  = []
        for data_message in data_messages:
            if data_message == '':
                continue
            log_message = LogMessage()
            log_message.level = int(data_message[0])
            log_message.data = data_message[1:] + '\n'
            self.LogMessages.append(log_message)
            logMessages.append(log_message)
        self.AppendToText(logMessages)

    def AppendToText(self, logMessages):
        for message in logMessages:
            if self.filters.IsValidForFilter(message):
                self.FilteredLogMessages.append(message)
                self.DisplayLogMessage(message.level, message.data)
    
    def UpdateDisplayMessages(self):
        self.ClearText()
        self.FilteredLogMessages.clear()
        for message in self.LogMessages:
            if self.filters.IsValidForFilter(message):
                self.FilteredLogMessages.append(message)
        self.DisplayLogMessages()

    def ClearText(self):
        self.text.configure(state = tk.NORMAL)
        self.text.delete('0.0', tk.END)
        self.text.configure(state = tk.DISABLED)

    def ClearAllText(self):
        self.LogMessages = []
        self.text.configure(state = tk.NORMAL)
        self.text.delete('0.0', tk.END)
        self.text.configure(state = tk.DISABLED)

    def DisplayLogMessages(self):
        for log_message in self.FilteredLogMessages:
            self.DisplayLogMessage(log_message.level, log_message.data)

    def DisplayLogMessage(self, logId, message):
        self.text.configure(state = tk.NORMAL)
        first = self.text.index(tk.INSERT)
        try:
            level_message =  '[' + self.loglevels.dictionary[logId] + ']: '
        except:
            level_message = ''
        display_message = level_message + message
        self.text.insert(tk.END, display_message)
        end = self.text.index(tk.INSERT)
        # self.__SetFont(logId, first, end)
        self.text.configure(state = tk.DISABLED)

    def __SetFont(self, logId, first, end):
        if logId == 0:
            self.__SetDebugFont(first, end)
        elif logId == 1:
            self.__SetWarningFont(first, end)
        elif logId == 2:
            self.__SetErrorFont(first, end)
        elif logId == 3:
            self.__SetFatalFont(first, end)
        else:
            self.__SetUnkownFont(first, end)

    def __SetDebugFont(self, first, end):
        debugFont = font.Font(self.text, self.text.cget('font'))
        self.text.tag_configure(
            'debugFont', font=debugFont, background='#204F20')
        self.text.tag_add('debugFont', first, end)

    def __SetWarningFont(self, first, end):
        warningFont = font.Font(self.text, self.text.cget('font'))
        self.text.tag_configure(
            'warningFont', font=warningFont, background='#4F3F03')
        self.text.tag_add('warningFont', first, end)

    def __SetErrorFont(self, first, end):
        errorFont = font.Font(self.text, self.text.cget('font'))
        self.text.tag_configure(
            'errorFont', font=errorFont, background='#4F2020')
        self.text.tag_add('errorFont', first, end)

    def __SetFatalFont(self, first, end):
        fatalFont = font.Font(self.text, self.text.cget('font'))
        self.text.tag_configure(
            'fatalFont', font=fatalFont, background='#7F2020')
        self.text.tag_add('fatalFont', first, end)

    def __SetUnkownFont(self, first, end):
        unknownFont = font.Font(self.text, self.text.cget('font'))
        self.text.tag_configure(
            'unknownFont', font=unknownFont, background='#505050')
        self.text.tag_add('unknownFont', first, end)

win = tk.Tk()
win.title('Log Application')
win.columnconfigure(0, weight = 1)
win.rowconfigure(0, weight = 1)
win.configure(bg = "#303030")


application = LoggerApplication(win)

win.mainloop()