# convert onnx model to trt engines
# trtexec --onnx=save_weights/gesture.onnx \
#         --explicitBatch \
#         --saveEngine=engines/gesture.trt \
#         --minShapes=input:1x1x30x30 \
#         --optShapes=input:8x1x30x30 \
#         --maxShapes=input:16x1x30x30

# convert onnx model to trt engines
trtexec --onnx=save_weights/action.onnx \
        --explicitBatch \
        --saveEngine=engines/action.trt \
        --minShapes=input:1x1x30x30 \
        --optShapes=input:8x1x30x30 \
        --maxShapes=input:16x1x30x30
