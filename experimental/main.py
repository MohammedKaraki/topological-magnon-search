from config.config_pb2 import GlobalConfig
from magnon.config.config_pb2 import GlobalConfig
from magnon.config.read_global_config import read_global_config

def main():
    import os
    print("PATH: ", os.path.realpath(__file__))
    print("HHHHello!")


if __name__ == "__main__":
    main()
