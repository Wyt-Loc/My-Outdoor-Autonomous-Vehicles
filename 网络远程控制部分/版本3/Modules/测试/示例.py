def update_array(arr, row, col):
    # 获取二维数组的行数和列数
    num_rows = len(arr)
    num_cols = len(arr[0])

    # 将指定点周围的元素改为1
    for i in range(max(0, row - 1), min(num_rows, row + 2)):
        for j in range(max(0, col - 1), min(num_cols, col + 2)):
            arr[i][j] = 1

    return arr


# 创建一个全为0的二维数组
rows = 10
cols = 10
arr = [[0] * cols for _ in range(rows)]

# 将指定点(3, 4)周围的元素改为1
updated_arr = update_array(arr, 3, 4)

# 打印更新后的二维数组
for row in updated_arr:
    print(row)
