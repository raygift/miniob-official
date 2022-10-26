#! /bin/python

import fileinput
from hashlib import new


class colors:
    # Reset
    Null = '\033[0m'       # Text Reset
    # Regular Colors
    Black = '\033[0;30m'        # Black
    Red = '\033[0;31m'          # Red
    Green = '\033[0;32m'        # Green
    Yellow = '\033[0;33m'       # Yellow
    Blue = '\033[0;34m'         # Blue
    Purple = '\033[0;35m'       # Purple
    Cyan = '\033[0;36m'         # Cyan
    White = '\033[0;37m'        # White
    # Bold
    BBlack = '\033[1;30m'       # Black
    BRed = '\033[1;31m'         # Red
    BGreen = '\033[1;32m'       # Green
    BYellow = '\033[1;33m'      # Yellow
    BBlue = '\033[1;34m'        # Blue
    BPurple = '\033[1;35m'      # Purple
    BCyan = '\033[1;36m'        # Cyan
    BWhite = '\033[1;37m'       # White
    # Underline
    UBlack = '\033[4;30m'       # Black
    URed = '\033[4;31m'         # Red
    UGreen = '\033[4;32m'       # Green
    UYellow = '\033[4;33m'      # Yellow
    UBlue = '\033[4;34m'        # Blue
    UPurple = '\033[4;35m'      # Purple
    UCyan = '\033[4;36m'        # Cyan
    UWhite = '\033[4;37m'       # White
    # Background
    On_Black = '\033[40m'       # Black
    On_Red = '\033[41m'         # Red
    On_Green = '\033[42m'       # Green
    On_Yellow = '\033[43m'      # Yellow
    On_Blue = '\033[44m'        # Blue
    On_Purple = '\033[45m'      # Purple
    On_Cyan = '\033[46m'        # Cyan
    On_White = '\033[47m'       # White


class log:
    def __init__(self) -> None:
        self.meta = {}
        self.date = ''
        self.time = ''
        self.level = 'info'
        self.file = ''
        self.func = ''
        self.line = ''
        self.content = ''


with fileinput.input() as f:
    meta_len = 45  # length of meta
    datetime_interval = 10  # every interval time print data&time

    c = colors()
    not_log_color = c.UYellow

    log_date_color = c.BYellow
    log_time_color = c.BYellow
    log_level_info_color = c.BGreen
    log_level_warn_color = c.BRed
    log_level_other_color = c.BRed
    log_file_color = c.Blue
    log_func_color = c.Blue
    log_content_color = c.Black

    last_log = log()
    lc = 0

    for line in f:
        # print("###origin###: " + line, end='')
        if line[0] != '[' or len(line.split(" ")) < 4:  # magic number
            # print(not_log_color + line + c.Null, end='')
            continue
        line = line.replace(']>>', ' ')
        line = line.replace('[', '')
        fields = line.split(' ')
        l = log()
        l.date = fields[0]
        l.time = fields[1]
        l.meta["pid"] = fields[2]
        l.meta["tid"] = fields[3]
        l.level = fields[5][:-1]
        l.file = fields[6]
        l.func = fields[7]
        l.line = fields[8]
        l.content = " ".join(fields[9:])

        # print('#'.join(fields))

        if l.file in ["log_reporter.cpp", "mem_pool.h", "thread_pool.cpp"]:
            continue

        if (last_log.date == l.date and last_log.time == l.time) and lc < 10:
            pass
            lc += 1
        else:
            print(log_date_color + '======' + l.date + ' ' +
                  log_time_color + l.time + "======" + c.Null)
            lc = 0

        wc = 0
        if l.level == 'INFO':
            print(log_level_info_color + '[INFO]' + c.Null, end='')
            wc += 6
        elif l.level == 'WARNNING':
            print(log_level_warn_color + '[WARN]' + c.Null, end='')
            wc += 6
        else:
            print(log_level_other_color + '[' + l.level + ']' + c.Null, end='')
            wc += 2 + len(l.level)

        print(log_file_color + l.file +
              '(' + l.line + ')' + log_func_color + l.func + c.Null, end='')
        wc += len(l.file) + len(l.func) + len(l.line) + 3

        for _ in range(0, meta_len - wc):
            print(" ", end='')

        if l.content.find('Observer start success') != -1:
            print(log_level_info_color + l.content + c.Null, end='')
        elif l.content.find('Shutdown Cleanly!') != -1:
            print(log_level_warn_color + l.content + c.Null, end='')
        elif l.level == 'WARNNING':
            print(log_level_warn_color + l.content + c.Null, end='')
        else:
            print(log_content_color + l.content + c.Null, end='')
        last_log = l
