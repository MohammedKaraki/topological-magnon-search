import sys, os
print(sys.path, 'aaa')
print(os.environ['PYTHONPATH'], 'bbb')
from magnon.config.config_pb2 import GlobalConfig
def read_global_config():
    return "ASAS"
