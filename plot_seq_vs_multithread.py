import os
import glob
import re
import pandas as pd
import matplotlib.pyplot as plt

def main():
    csv_files = glob.glob('csv/*.csv')
    if not csv_files:
        print("No CSV files found in the 'csv/' directory.")
        return

    data_summary = {}

    for f in csv_files:
        df = pd.read_csv(f)
        filename = os.path.basename(f)
        
        match = re.search(r'(\d+)KB', filename)
        if not match: continue
        size_kb = int(match.group(1))

        if size_kb not in data_summary:
            data_summary[size_kb] = {'sequential': None, 'multithread_time': None, 'threads': 0}

        if 'sequential' in filename.lower():
            data_summary[size_kb]['sequential'] = df['time'].iloc[0]
        elif 'multithread' in filename.lower():
            max_thread_time = df['time'].max()
            data_summary[size_kb]['multithread_time'] = max_thread_time
            data_summary[size_kb]['threads'] = len(df)

    sizes = sorted(list(data_summary.keys()))
    seq_times = [data_summary[s]['sequential'] for s in sizes]
    mt_times = [data_summary[s]['multithread_time'] for s in sizes]
    thread_counts = [data_summary[s]['threads'] for s in sizes]

    fig, ax = plt.subplots(figsize=(10, 6))

    ax.plot(sizes, seq_times, marker='o', linestyle='-', color='#d62728', linewidth=2.5, markersize=8, label='Sequential')
    ax.plot(sizes, mt_times, marker='s', linestyle='-', color='#2ca02c', linewidth=2.5, markersize=8, label='Multithread')

    for i, txt in enumerate(thread_counts):
        if txt > 0:
            ax.annotate(f"{txt} threads", (sizes[i], mt_times[i]), textcoords="offset points", xytext=(0,10), ha='center', fontsize=10)

    ax.set_xlabel('Experiment Data Goal (KB)', fontsize=12, fontweight='bold')
    ax.set_ylabel('Total Time (seconds)', fontsize=12, fontweight='bold')
    ax.set_title('Sequential vs. Multithread Performance', fontsize=15, fontweight='bold')
    ax.grid(True, linestyle='--', alpha=0.7)
    ax.legend(fontsize=12)

    ax.set_xticks(sizes)
    ax.set_xticklabels([f"{s}KB" for s in sizes])

    plt.tight_layout()
    output_path = 'csv/sequential_vs_multithread.png'
    plt.savefig(output_path, dpi=300)

if __name__ == "__main__":
    main()