from magnon.config.config_pb2 import GlobalConfig
from magnon.config.read_global_config_python import read_global_config

def main():
    print(read_global_config())


if __name__ == "__main__":
    main()
