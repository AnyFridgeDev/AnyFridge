import unittest
from fastapi.testclient import TestClient
import uvicorn
from common import delete_user
from request_handler import anyfridge_request_handler
import random 


class TestRequestHandling(unittest.TestCase):
    """Test cases for the request handling API"""

    @staticmethod
    def code_in_scan(user_data, upc_code):
        """Check if a given UPC code is in the list of scans"""
        for scan in user_data["scans"]:
            if scan["upc_code"] == upc_code:
                return True
        return False

    @classmethod
    def setUpClass(cls):
        """Set up the test client before running tests"""
        delete_user("test_user")
        cls.client = TestClient(anyfridge_request_handler)

    def test_0_post_upc(self):
        """Test adding a UPC code"""
        response = self.client.post("/update", json={"user_id": "test_user", "upc_code": "123456789012", "action": "POST"})
        self.assertEqual(response.status_code, 200)
        user_data = response.json()["data"]
        self.assertTrue(self.code_in_scan(user_data, "123456789012"))

    def test_1_get_user_data(self):
        """Test retrieving UPCs for a user"""
        response = self.client.get("/get/test_user")
        self.assertEqual(response.status_code, 200)
        print(response.json())
        self.assertTrue(self.code_in_scan(response.json(), "123456789012"))

    def test_2_delete_upc(self):
        """Test deleting a UPC code"""
        response = self.client.post("/update", json={"user_id": "test_user", "upc_code": "123456789012", "action": "DELETE"})
        self.assertEqual(response.status_code, 200)
        self.assertFalse(self.code_in_scan(response.json()["data"], "123456789012"))

    def test_3_delete_nonexistent_upc(self):
        """Test deleting a UPC that does not exist"""
        response = self.client.post("/update", json={"user_id": "test_user", "upc_code": "999999999999", "action": "DELETE"})
        self.assertEqual(response.status_code, 404)

    def test_4_invalid_action(self):
        """Test invalid action handling"""
        response = self.client.post("/update", json={"user_id": "test_user", "upc_code": "111111111111", "action": "UPDATE"})
        self.assertEqual(response.status_code, 400)

    def test_5_create_sample_user(self):
        """Test creating a sample user"""
        delete_user("sample_user")
        for _ in range(10):
            num = ""
            for _ in range(12):
                num += str(random.randint(0, 9))
            response = self.client.post("/update", json={"user_id": "sample_user", "upc_code": num, "action": "POST"})
            self.assertEqual(response.status_code, 200)
        response = self.client.get("/get/sample_user")
        self.assertEqual(len(response.json()["scans"]), 10)

if __name__ == "__main__":
    unittest.main()
