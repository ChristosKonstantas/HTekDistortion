import numpy as np
import matplotlib.pyplot as plt

def quartic(u):
    return u - u**3 + 1/2 * u **4

def waveshape(x, threshold, knee_frac):
    x = np.asarray(x, dtype=np.float32)
    t = np.clip(threshold, 0.01, 1.0)
    k = np.clip(knee_frac, 0.0, 0.5)

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
        q = quartic(u)
        yk = a + (b-a) * q
        y[m2] = sign[m2] * yk

    return y

if __name__ == "__main__":
    xs = np.linspace(-1, 1, 100000)
    ys = waveshape(xs, threshold=0.4, knee_frac=0.0)
    print(np.max(np.abs(waveshape(-xs, 0.4, 0.0) + ys)))
    
    # (1) Transfer curves y = f(x)
    xs = np.linspace(-1.2, 1.2, 4000)

    settings = [
        (0.7, 0.0),  # hard clip at +-0.7
        (0.6, 0.3),  # softer knee
        (0.4, 0.5),  # wide knee
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
    buffer_size_samples = 256
    duration_seconds = buffer_size_samples / fs
    total_samples = int(fs * duration_seconds)
    tvec = np.arange(total_samples) / fs
    tvec_ms = tvec * 1000

    # select settings
    threshold = settings[1][0]
    knee = settings[1][1]
    x = 0.9 * np.sin(2*np.pi*f*tvec)
    y = waveshape(x, threshold=threshold, knee_frac=knee)

    plt.figure()
    plt.plot(tvec_ms, x, label="input")
    plt.plot(tvec_ms, y, label=f"output (threshold={threshold}, knee={knee})")
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
    knee = settings[1][1]
    y = waveshape(x, threshold=threshold, knee_frac=knee)

    w = np.blackman(n)
    X = np.fft.rfft(x * w)
    Y = np.fft.rfft(y * w)
    freqs = np.fft.rfftfreq(n, 1/fs)

    plt.figure()
    plt.semilogx(freqs[1:], 20*np.log10(np.abs(X[1:]) + 1e-12), label="input")
    plt.semilogx(freqs[1:], 20*np.log10(np.abs(Y[1:]) + 1e-12), label=f"output (threshold={threshold}, knee={knee})")
    plt.title("Spectrum: input vs output (harmonics from distortion)")
    plt.xlabel("frequency (Hz)")
    plt.ylabel("magnitude (dB)")
    plt.grid(True, which="both")
    plt.legend()
    plt.show()