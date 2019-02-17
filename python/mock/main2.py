import os
import unittest
from unittest import mock
import es


def mocked_search(*args, **kwargs):



class TestEs(unittest.TestCase):

    @mock.patch.object(, side_effect=mocked_search)
    def test_search(self, ):
        e = es.ElasticsearchHandler("permonitor", 9200)



if __name__ == "__main__":
    unittest.main()
