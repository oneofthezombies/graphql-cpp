FROM mcr.microsoft.com/devcontainers/base:debian-12

ENV DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]

RUN apt update && \
    apt install -y \
    curl \
    git \
    # libstdc++
    build-essential \
    # GDB
    gdb \
    # To open in a browser on the host
    xdg-utils \
    # LLVM
    lsb-release \
    wget \
    software-properties-common \
    gnupg \
    && curl -fsSL "https://apt.llvm.org/llvm.sh" | bash -s 19 \
    && apt install -y \
    clang-19 \
    clang-format-19 \
    lld-19 \
    # For cmake-format
    pipx \
    # For ninja
    ninja-build \
    && apt clean && \
    rm -rf "/var/lib/apt/lists/*" && \
    ln -s "/usr/lib/llvm-19" "/opt/llvm"

ENV CMAKE_VERSION="3.31.5"
RUN ARCH="$(uname -m)" && \
    CMAKE_DIR="cmake-$CMAKE_VERSION-linux-$ARCH" && \
    CMAKE_FILENAME="$CMAKE_DIR.tar.gz" && \
    curl -fsSLO "https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/$CMAKE_FILENAME" && \
    tar -C /opt -xf "$CMAKE_FILENAME" && \
    rm "$CMAKE_FILENAME" && \
    ln -s "/opt/$CMAKE_DIR" "/opt/cmake"

USER vscode
ENV HOME="/home/vscode"
RUN echo '' >> ~/.bashrc && \
    echo '################################################################################' >> ~/.bashrc && \
    echo '# Environment variables' >> ~/.bashrc && \
    echo '################################################################################' >> ~/.bashrc && \
    echo '' >> ~/.bashrc && \
    echo '# C++' >> ~/.bashrc && \
    echo 'export PATH="/opt/llvm/bin:$PATH"' >> ~/.bashrc && \
    echo 'export PATH="/opt/cmake/bin:$PATH"' >> ~/.bashrc

ENV PATH="$HOME/.volta/bin:$PATH"
RUN echo '# JavaScript' >> ~/.bashrc && \
    curl https://get.volta.sh | bash && \
    volta install \
    node@22.13.0 \
    npm@11.0.0 \
    gitmoji-cli@9.5.0 \
    && echo '' >> ~/.bashrc

RUN pipx ensurepath && \
    source ~/.bashrc && \
    pipx install \
    # For cmake-format
    cmakelang==0.6.13

RUN echo '# Aliases' >> ~/.bashrc && \
    echo 'alias ll="ls -alhF --color=auto"' >> ~/.bashrc && \
    echo '' >> ~/.bashrc

WORKDIR /work
