FROM debian:stable-slim

# Install necessary dependencies (GCC, make, python3, venv, pip)
RUN apt-get update && apt-get install -y gcc make python3 python3-venv python3-pip

WORKDIR /app

# Create a virtual environment
RUN python3 -m venv venv

# Copy requirements.txt BEFORE activating venv and installing
COPY requirements.txt ./

# Activate the virtual environment and install dependencies
RUN . venv/bin/activate && pip3 install --no-cache-dir -r requirements.txt

# Copy the Makefile and source code
COPY Makefile ./
COPY src/ ./src/

# Build the C program
RUN . venv/bin/activate && make chess_program

# Copy the FastAPI script
COPY app.py ./

# Run the FastAPI application using uvicorn
CMD ["venv/bin/uvicorn", "app:app", "--host", "0.0.0.0", "--port", "5000"]


