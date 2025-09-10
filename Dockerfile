# Use Ubuntu 24.04 (newer toolchain)
FROM ubuntu:24.04

# Install build tools
RUN apt-get update && apt-get install -y \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy all source code
COPY . .

# Build inside the container
RUN make

# Default command runs the server
CMD ["./chat_server"]