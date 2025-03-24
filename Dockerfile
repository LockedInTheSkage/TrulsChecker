# Use an official Debian based image as a parent image
FROM debian:stable-slim

# Install necessary dependencies (GCC and make)
RUN apt-get update && apt-get install -y gcc make

# Create a working directory inside the container
WORKDIR /app

# Copy the source code to the container
COPY src/ ./src/

# Copy the Makefile to the correct location
COPY Makefile ./

# Build the executable
RUN make

# Command to run when the container starts
CMD ["./game"]