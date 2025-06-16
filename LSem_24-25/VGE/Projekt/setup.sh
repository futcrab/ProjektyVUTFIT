#!/bin/bash

VENV_DIR="VisualizerEnv"
ACTIVATE="$VENV_DIR/bin/activate"

create_venv() {
    if [ -d "$VENV_DIR" ]; then
        echo "Virtual environment '$VENV_DIR' already exists."
    else
        echo "Creating virtual environment..."
        python3 -m venv "$VENV_DIR" || exit 1
        echo "Virtual environment created."
    fi

    echo "Activating virtual environment and installing dependencies..."
    source "$ACTIVATE"

    # Ensure pip is available and up to date
    python -m ensurepip --upgrade
    python -m pip install --upgrade pip

    # Install packages
    pip install open3d polyscope numpy scipy

    deactivate
    echo "Dependencies installed and environment deactivated."
}

delete_venv() {
    if [ -d "$VENV_DIR" ]; then
        echo "Deleting virtual environment..."
        rm -rf "$VENV_DIR"
        echo "Virtual environment deleted."
    else
        echo "Virtual environment not found."
    fi
}

case "$1" in
    create)
        create_venv
        ;;
    delete)
        delete_venv
        ;;
    *)
        echo "Usage: $0 {create|delete}"
        ;;
esac