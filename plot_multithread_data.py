import sys
import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def main():
    if len(sys.argv) < 2:
        print("Error: CSV file path was not specified -_-")
        sys.exit(1)

    csv_path = sys.argv[1]
    filename = os.path.basename(csv_path)

    try:
        df = pd.read_csv(csv_path)
    except FileNotFoundError:
        print(f"Error: Could not find {csv_path}")
        sys.exit(2)

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
    fig.suptitle(f"Thread vs. Time\n({filename})", fontsize=16, fontweight='bold')

    ax1.scatter(df['n_files'], df['time'], color='#ff7f0e', s=100, edgecolors='black', zorder=3)
    ax1.set_xlabel('Number of Files', fontsize=12, fontweight='bold')
    ax1.set_ylabel('Time (seconds)', fontsize=12, fontweight='bold')
    ax1.set_title('Time vs. Number of Files', fontsize=13)
    ax1.grid(True, linestyle='--', alpha=0.7, zorder=0)

    if df['n_files'].nunique() > 1:
        z = np.polyfit(df['n_files'], df['time'], 1)
        p = np.poly1d(z)
        x_seq1 = np.linspace(df['n_files'].min(), df['n_files'].max(), 100)
        ax1.plot(x_seq1, p(x_seq1), "r--", alpha=0.8, label='Linear Regression')
        ax1.legend()

    ax2.scatter(df['data'], df['time'], color='#1f77b4', s=100, edgecolors='black', zorder=3)
    ax2.set_xlabel('Data Processed (Bytes)', fontsize=12, fontweight='bold')
    ax2.set_ylabel('Time (seconds)', fontsize=12, fontweight='bold')
    ax2.set_title('Time vs. Data Processed', fontsize=13)
    ax2.grid(True, linestyle='--', alpha=0.7, zorder=0)

    if df['data'].nunique() > 1:
        z2 = np.polyfit(df['data'], df['time'], 1)
        p2 = np.poly1d(z2)
        x_seq2 = np.linspace(df['data'].min(), df['data'].max(), 100)
        ax2.plot(x_seq2, p2(x_seq2), "r--", alpha=0.8, label='Linear Regression')
        ax2.legend()
        
    ax2.xaxis.set_major_formatter(plt.FuncFormatter(lambda x, loc: "{:,}".format(int(x))))
    
    plt.tight_layout()
    output_png = csv_path.replace('.csv', '.png')
    plt.savefig(output_png, dpi=300, bbox_inches='tight')

if __name__ == "__main__":
    main()