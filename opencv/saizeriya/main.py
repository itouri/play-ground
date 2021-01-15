import cv2
import math
import numpy as np

# --------------------------------------------------- #
# 画像合成                                             #
# --------------------------------------------------- #


def ImageComposition(img2, result):
    img3 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)  # グレースケール化
    img3 = cv2.cvtColor(img3, cv2.COLOR_GRAY2BGR)  # グレースケールのままカラー画像にする
    # コントラスト、明るさを変更する。
    img3 = adjust(img3, alpha=0.25)
    add = cv2.add(img3, result)  # 画像を合成する
    return add

# α  はゲイン (gain) 、βはバイアス (bias)


def adjust(img, alpha=1.0, beta=0.0):
    # 積和演算を行う。
    dst = alpha * img + beta
    # [0, 255] でクリップし、uint8 型にする。
    return np.clip(dst, 0, 255).astype(np.uint8)


def FitImageSize_small(img1, img2):
    # height
    if img1.shape[0] > img2.shape[0]:
        height = img2.shape[0]
        width = img1.shape[1]
        img1 = cv2.resize(img1, (width, height))
    else:
        height = img1.shape[0]
        width = img2.shape[1]
        img2 = cv2.resize(img2, (width, height))

    # width
    if img1.shape[1] > img2.shape[1]:
        height = img1.shape[0]
        width = img2.shape[1]
        img1 = cv2.resize(img1, (width, height))
    else:
        height = img2.shape[0]
        width = img1.shape[1]
        img2 = cv2.resize(img2, (width, height))
    return img1, img2


try:
    img = cv2.imread('./img/2009.png')

    if img is None:
        print('ファイルを読み込めません')
        import sys
        sys.exit()

    cv2.imshow("img", img)

    # 余白を取り除いたときに2つの画像が最も一致するような適切な余白（padding）の幅を見つける
    img_src = img
    img_diffs = []
    paddings = []
    for padding in range(1, 50):
        # 画像の余白を削除
        img = img_src[:, padding:-padding]

        # 画像を左右で分割する
        height, width, channels = img.shape[:3]
        img1 = img[:, :width//2]
        img2 = img[:, width//2:]

        # 画像サイズを合わせる(小さい方に)
        img1, img2 = FitImageSize_small(img1, img2)

        # 2つの画像の差分を算出
        img_diff = cv2.absdiff(img2, img1)
        img_diff_sum = np.sum(img_diff)

        img_diffs.append((img_diff, img_diff_sum))
        paddings.append(padding)

    # 差分が最も少ないものを選ぶ
    img_diff, _ = min(img_diffs, key=lambda x: x[1])
    index = img_diffs.index(min(img_diffs, key=lambda x: x[1]))
    cv2.imshow("img_diff", img_diff)

    padding = paddings[index]
    # 画像の余白を削除
    img = img_src[:, padding:-padding]
    # 画像を左右で分割する
    height, width, channels = img.shape[:3]
    img1 = img[:, :width//2]
    img2 = img[:, width//2:]
    cv2.imshow("img2", img2)

    # 画像サイズを合わせる(小さい方に)
    img2, img_diff = FitImageSize_small(img2, img_diff)
    # 画像合成
    add = ImageComposition(img2, img_diff)
    cv2.imshow("add", add)

    cv2.waitKey(0)
    cv2.destroyAllWindows()
except:
    import sys
    print("Error:", sys.exc_info()[0])
    print(sys.exc_info()[1])
    import traceback
    print(traceback.format_tb(sys.exc_info()[2]))
