# --- 5.1 Insertion ---




def bit_mask(i,j):
    # 111...1110000111...1111
    #         j    i
    ones = 0xffffffff
    right_ones = ones >> 32-i
    left_ones = ones << j + 1
    mask = (left_ones | right_ones) & ones
    return mask
    
    
def insertion(n, m, i, j):
    return (n & bit_mask(i,j)) | (m << i)
