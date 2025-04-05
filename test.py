import subprocess
import requests
import json
import time

def test_c_program():
    """Tests the C program directly within the Docker container."""
    try:
        # Run the container in interactive mode and execute C program tests
        process = subprocess.run(
            ["docker", "run", "--rm", "trulschecker", "/bin/bash", "-c",
             "./chess_program --api 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1' && ./chess_program --train"],
            capture_output=True,
            text=True,
            check=True,
        )
        print("C Program Tests Passed!")
        print(process.stdout)
    except subprocess.CalledProcessError as e:
        print(f"C Program Tests Failed: {e}")
        print(e.stderr)
        exit(1)

def test_fastapi_endpoints_docker():
    """Tests the FastAPI endpoints."""
    try:
        # Run the Docker container in detached mode
        process = subprocess.Popen(
            ["docker", "run", "-p", "5000:5000", "--name", "trulschecker_test", "trulschecker"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )

        time.sleep(5) # Give the container time to start.

        # Test best_move endpoint
        response = requests.post(
            "http://localhost:5000/best_move",
            json={"fen": "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"},
        )
        response.raise_for_status()
        print(f"best_move: {response.json()}")

        # TODO: Training is disabled for now, so this test will fail and might mess up the other tests.
        # # Test train endpoint
        # response = requests.post("http://localhost:5000/train")
        # response.raise_for_status()
        # print(f"train: {response.json()}")

        # Test new-game endpoint
        response = requests.post("http://localhost:5000/new-game")
        response.raise_for_status()
        new_game_data = response.json()
        print(f"new-game: {new_game_data}")

        # Test load-game endpoint
        response = requests.get(f"http://localhost:5000/load-game?browser_id={new_game_data['browser_id']}")
        response.raise_for_status()
        print(f"load-game: {response.json()}")

        # Test legal-moves endpoint
        response = requests.get(f"http://localhost:5000/legal-moves/e2?browser_id={new_game_data['browser_id']}")
        response.raise_for_status()
        print(f"legal-moves: {response.json()}")

        # Test make-move endpoint
        response = requests.get(f"http://localhost:5000/make-move/e2/e4?browser_id={new_game_data['browser_id']}")
        response.raise_for_status()
        print(f"make-move: {response.json()}")

        print("FastAPI Endpoint Tests Passed!")

    except requests.exceptions.RequestException as e:
        print(f"FastAPI Endpoint Tests Failed: {e}")
        exit(1)
    except Exception as e:
        print(f"FastApi endpoint test failed: {e}")
        exit(1)
    finally:
        # Stop and remove the container
        subprocess.run(["docker", "stop", "trulschecker_test"], capture_output=True)
        subprocess.run(["docker", "rm", "trulschecker_test"], capture_output=True)

def test_fastapi_endpoints():
    """Tests the FastAPI endpoints."""
    try:

        # TODO: Training is disabled for now, so this test will fail and might mess up the other tests.
        # # Test train endpoint
        # response = requests.post("http://localhost:5000/train")
        # response.raise_for_status()
        # print(f"train: {response.json()}")

        # Test new-game endpoint
        response = requests.post("http://localhost:5000/new-game")
        response.raise_for_status()
        new_game_data = response.json()
        print(f"new-game: {new_game_data}")

        # Test load-game endpoint
        response = requests.get(f"http://localhost:5000/load-game?browser_id={new_game_data['browser_id']}")
        response.raise_for_status()
        print(f"load-game: {response.json()}")

        # Test legal-moves endpoint
        response = requests.get(f"http://localhost:5000/legal-moves/e2?browser_id={new_game_data['browser_id']}")
        response.raise_for_status()
        print(f"legal-moves: {response.json()}")

        # Test make-move endpoint
        response = requests.get(f"http://localhost:5000/make-move/e2/e4?browser_id={new_game_data['browser_id']}")
        response.raise_for_status()
        print(f"make-move: {response.json()}")

        print("FastAPI Endpoint Tests Passed!")

    except requests.exceptions.RequestException as e:
        print(f"FastAPI Endpoint Tests Failed: {e}")
        exit(1)
    except Exception as e:
        print(f"FastApi endpoint test failed: {e}")
        exit(1)
    finally:
        # Stop and remove the container
        subprocess.run(["docker", "stop", "trulschecker_test"], capture_output=True)
        subprocess.run(["docker", "rm", "trulschecker_test"], capture_output=True)


if __name__ == "__main__":
    #test_c_program()
    test_fastapi_endpoints()
    print("All tests passed!")