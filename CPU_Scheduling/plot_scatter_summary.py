import pandas as pd
import matplotlib.pyplot as plt

# Load the summary file
df = pd.read_csv('summary.csv')

# Add a column for Run number (assuming one line per algorithm per run)
df['Run'] = df.groupby('Algorithm').cumcount()

# Pivot for easier plotting
pivot_wt = df.pivot(index='Run', columns='Algorithm', values='AvgWaitingTime')
pivot_tat = df.pivot(index='Run', columns='Algorithm', values='AvgTurnaroundTime')
pivot_rt = df.pivot(index='Run', columns='Algorithm', values='AvgResponseTime')

# Define color mapping
colors = {
    'RR': 'blue',
    'SRTF': 'green',
    'FCFS': 'red',
    'MLFQ': 'orange'
}

# Plot 1: Avg Waiting Time
plt.figure(figsize=(10, 5))
for algo in pivot_wt.columns:
    plt.plot(pivot_wt.index, pivot_wt[algo], marker='o', label=algo, color=colors[algo])
plt.title('Average Waiting Time')
plt.xlabel('Run')
plt.ylabel('Time')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("avg_waiting_time_per_run.png")
plt.close()

# Plot 2: Avg Turnaround Time
plt.figure(figsize=(10, 5))
for algo in pivot_tat.columns:
    plt.plot(pivot_tat.index, pivot_tat[algo], marker='o', label=algo, color=colors[algo])
plt.title('Average Turnaround Time')
plt.xlabel('Run')
plt.ylabel('Time')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("avg_turnaround_time_per_run.png")
plt.close()

# Plot 3: Avg Response Time
plt.figure(figsize=(10, 5))
for algo in pivot_rt.columns:
    plt.plot(pivot_rt.index, pivot_rt[algo], marker='o', label=algo, color=colors[algo])
plt.title('Average Response Time')
plt.xlabel('Run')
plt.ylabel('Time')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("avg_response_time_per_run.png")
plt.close()

print("✅ Saved: avg_waiting_time_per_run.png, avg_turnaround_time_per_run.png, avg_response_time_per_run.png")
