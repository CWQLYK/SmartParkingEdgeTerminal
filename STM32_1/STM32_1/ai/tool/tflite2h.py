# tflite转C数组头文件，替代xxd -i
input_file = "park_model.tflite"   # 你的模型名
output_file = "park_data.h"        # 输出头文件名

with open(input_file, "rb") as f:
    data = f.read()

# 生成C数组格式
array_name = input_file.replace(".", "_")
with open(output_file, "w") as f:
    f.write(f"unsigned char {array_name}[] = {{\n")
    for i, byte in enumerate(data):
        if i % 12 == 0:
            f.write("    ")
        f.write(f"0x{byte:02x}, ")
        if i % 12 == 11:
            f.write("\n")
    f.write(f"\n}};\nunsigned int {array_name}_len = {len(data)};")

print(f"✅ 生成成功：{output_file}")