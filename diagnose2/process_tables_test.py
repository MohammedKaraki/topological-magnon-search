import unittest

from magnon.diagnose2.process_tables import process_tables

"""Warning: these tests are not hermetic and require internet access (when no cached data is
available). Thus, they may break for external reasons (e.g. changes to the BCS data online).

TODO: Refactor the fetchers to allow isolated tests.
"""


class ProcessTablesTest(unittest.TestCase):
    def empty_test(self):
        pass


if __name__ == "__main__":
    unittest.main()
