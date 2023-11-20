ARG PARENT_IMAGE_NAME

FROM ${PARENT_IMAGE_NAME}:latest

ARG NUMPY_VERSION

# Install numpy ##
RUN pip install numpy
