import numpy as np
import matplotlib.pyplot as plt

def smoothstep(u):
    u = np.clip(u, 0.0, 1.0)
    return u*u*(3 - 2*u)


def smootherstep(u):
    u = np.clip(u, 0.0, 1.0)
    return  u*u*u*(u*(u*6 - 15) + 10)


def waveshape(x, threshold, knee_frac):
    x = np.asarray(x, dtype=np.float32)
    t = np.clip(threshold, 0.01, 1.0)
    k = np.clip(knee_frac, 0.0, 0.49)

    a = t * (1.0 - k)   # knee start
    b = t * (1.0 + k)   # knee end

    ax = np.abs(x)
    sign = np.sign(x)
    sign = np.where(sign == 0.0, 1.0, sign)

    y = np.empty_like(x)

    # region 1: linear
    m1 = ax <= a
    y[m1] = x[m1]

    # region 3: hard clip
    m3 = ax >= b
    y[m3] = sign[m3] * t

    # region 2: knee
    m2 = (~m1) & (~m3)
    if np.any(m2):
        u = (ax[m2] - a) / (b - a)
        u = np.clip(u, 0.0, 1.0)
        s = smootherstep(u)
        yk = (1.0 - s) * ax[m2] + s * t
        y[m2] = sign[m2] * yk

    return y

if __name__ == "__main__":
    xs = np.linspace(-1, 1, 100000)
    ys = waveshape(xs, threshold=0.4, knee_frac=0.0)
    print(np.max(np.abs(waveshape(-xs, 0.4, 0.0) + ys)))
    
    # (1) Transfer curves y = f(x)
    xs = np.linspace(-1.2, 1.2, 4000)

    settings = [
        (0.6, 0.0),  # hard clip at +-0.6
        (0.6, 0.1),  # slightly softer knee
        (1, 0.5),    # wide knee
        (0.3, 0.1),  # lower threshold = heavier distortion
    ]

    plt.figure()
    for thr, knee in settings:
        ys = waveshape(xs, thr, knee)
        plt.plot(xs, ys, label=f"threshold={thr}, knee={knee}")
    plt.title("Transfer curves (y = f(x))")
    plt.xlabel("input x")
    plt.ylabel("output y")
    plt.grid(True)
    plt.legend()
    plt.show()

    # (2) Time domain: sine before/after
    fs = 48000
    f = 200
    dur = 256 / 48000
    n = int(fs * dur)
    tvec = np.arange(n) / fs

    x = 0.9 * np.sin(2*np.pi*f*tvec)
    y = waveshape(x, threshold=0.5, knee_frac=0.4)

    plt.figure()
    plt.plot(tvec*1000, x, label="input")
    plt.plot(tvec*1000, y, label="output")
    plt.title("Time domain: sine before/after waveshaping")
    plt.xlabel("time (ms)")
    plt.ylabel("amplitude")
    plt.grid(True)
    plt.legend()
    plt.show()

    # (3) Frequency domain: observe harmonics added by distortion
    dur = 1.0
    n = int(fs * dur)
    tvec = np.arange(n) / fs

    x = 0.9 * np.sin(2*np.pi*f*tvec)
    y = waveshape(x, threshold=0.4, knee_frac=0.0)

    w = np.blackman(n)
    X = np.fft.rfft(x * w)
    Y = np.fft.rfft(y * w)
    freqs = np.fft.rfftfreq(n, 1/fs)

    plt.figure()
    plt.semilogx(freqs[1:], 20*np.log10(np.abs(X[1:]) + 1e-12), label="input")
    plt.semilogx(freqs[1:], 20*np.log10(np.abs(Y[1:]) + 1e-12), label="output")
    plt.title("Spectrum: input vs output (harmonics from distortion)")
    plt.xlabel("frequency (Hz)")
    plt.ylabel("magnitude (dB)")
    plt.grid(True, which="both")
    plt.legend()
    plt.show()

    Y = np.fft.rfft(y)
    freqs = np.fft.rfftfreq(n, 1/fs)
    plt.semilogx(freqs[1:], 20*np.log10(np.abs(Y[1:]) + 1e-12))
    plt.grid(True, which="both")
    plt.title("Spectrum: input vs output (harmonics from distortion)")
    plt.xlabel("frequency (Hz)")
    plt.ylabel("magnitude (linear)")
    plt.grid(True, which="both")
    plt.legend()
    plt.show()
    plt.show()