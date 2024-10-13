import numpy as np
import matplotlib.pyplot as plt
import sys

def read_benchmark_data(file_path):
    """Read benchmark data from a specified file using NumPy."""
    try:
        # Print file content for debugging
        with open(file_path, 'r') as file:
            print(file.read())
        
        # Load the data, skipping the header row
        data = np.loadtxt(file_path, skiprows=2)
        return data
    except ValueError as e:
        print(f"Error reading data from {file_path}: {e}")
        sys.exit(1)

def main(file_path):
    # Read data from the provided file
    data = read_benchmark_data(file_path)

    print(f"Data shape: {data.shape}")
    
    # Extract columns
    sizes = data[:, 0]  # Size
    cpu_times = data[:, 1]  # CPU Time
    gpu_times = data[:, 2]  # GPU Time

    # Calculate speedup
    speedup = cpu_times / gpu_times

    # Plotting
    plt.figure(figsize=(14, 8))

    # Plot CPU and GPU times
    plt.subplot(2, 1, 1)
    plt.plot(sizes, cpu_times, label='IrisPlus Time', marker='o')
    plt.plot(sizes, gpu_times, label='A100 Time', marker='s')
    plt.title('OpenCL GPU (Intel(R) Iris(TM) Plus Graphics 640) vs CUDA GPU (NVIDIA A100-SXM-64GB)')
    #plt.xlabel('MatMul Size (N^2)')
    plt.ylabel('Time [s]')
    plt.grid()
    plt.legend()

    # Plot speedup
    plt.subplot(2, 1, 2)
    plt.plot(sizes, speedup, label='Speedup (A100/IrisPlus)', color='orange', marker='x')
    plt.title('Speedup ratio')
    plt.xlabel('MatMul Size $(N^2)$')
    plt.ylabel('Speedup')
    plt.grid()
    plt.legend()

    plt.tight_layout()    #Save the plot
    plt.savefig('benchmark_plot_GPU_GPU.pdf')
    plt.show()



if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python benchmark_plot.py <data_file>")
        sys.exit(1)

    file_path = sys.argv[1]
    main(file_path)