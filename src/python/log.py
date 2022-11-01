import logging


FORMAT = "%(asctime)s [%(name)s] [%(levelname)s] %(message)s"


def create_root_logger(filename):
    logger = logging.getLogger()
    logger.setLevel(logging.DEBUG)

    class ColoredFormatter(logging.Formatter):

        grey = "\x1b[38;20m"
        bold_blue = "\x1b[1;34m"
        blue = "\x1b[36m"
        magenta = "\x1b[95m"
        yellow = "\x1b[91;1;44;20m"
        red = "\x1b[31;20m"
        bold_red = "\x1b[31;1m"
        reset = "\x1b[0m"

        FORMATS = {
            logging.DEBUG: blue + FORMAT + reset,
            logging.INFO: magenta + FORMAT + reset,
            logging.WARNING: yellow + FORMAT + reset,
            logging.ERROR: bold_red + FORMAT + reset,
            logging.CRITICAL: bold_red + FORMAT + reset
        }

        def format(self, record):
            log_fmt = self.FORMATS.get(record.levelno)
            formatter = logging.Formatter(log_fmt)
            return formatter.format(record)


    file_handler = logging.FileHandler(filename)
    file_handler.setFormatter(logging.Formatter(FORMAT))
    file_handler.setLevel(logging.INFO)
    logger.addHandler(file_handler)

    console_handler = logging.StreamHandler()
    console_handler.setFormatter(ColoredFormatter())
    logger.addHandler(console_handler)

    return logger


def create_logger(name):
    logger = logging.getLogger(name)
    logger.setLevel(logging.DEBUG)

    return logger
