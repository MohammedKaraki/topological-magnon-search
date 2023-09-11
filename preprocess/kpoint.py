from magnon.preprocess import log

logger = log.create_logger(__name__)


class KPoint:
    def __init__(self, msg_number, standard_coords):
        self._msg_number = msg_number
        self._standard_coords = standard_coords
