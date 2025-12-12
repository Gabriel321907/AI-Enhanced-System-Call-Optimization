import pandas as pd
import numpy as np
import argparse
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestRegressor
from sklearn.metrics import mean_squared_error
from sklearn.preprocessing import LabelEncoder
import joblib
import os

# -------------------------------
# Argument Parser
# -------------------------------
parser = argparse.ArgumentParser(description="AI Model for System Call Optimization")
parser.add_argument("--csv", required=True, help="Input syscall data CSV file")
parser.add_argument("--out", default="results", help="Output directory for results")
args = parser.parse_args()

csv_path = args.csv
output_dir = args.out

os.makedirs(output_dir, exist_ok=True)

# -------------------------------
# Load Dataset
# -------------------------------
print(f"Loading dataset from: {csv_path}")
df = pd.read_csv(csv_path)

# Expecting Columns:
# timestamp, pid, syscall, exec_time

print("\nDataset Head:")
print(df.head())

# -------------------------------
# Encode Categorical Feature (syscall name)
# -------------------------------
label_encoder = LabelEncoder()
df['syscall_encoded'] = label_encoder.fit_transform(df['syscall'])

# Features and target
X = df[['pid', 'syscall_encoded']]
y = df['exec_time']

# -------------------------------
# Train-Test Split
# -------------------------------
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.2, random_state=42
)

# -------------------------------
# Train Random Forest Model
# -------------------------------
model = RandomForestRegressor(n_estimators=120, random_state=42)
model.fit(X_train, y_train)

# -------------------------------
# Evaluate Model
# -------------------------------
y_pred = model.predict(X_test)
mse = mean_squared_error(y_test, y_pred)
rmse = np.sqrt(mse)

print(f"\nModel RMSE: {rmse:.4f} microseconds")

# -------------------------------
# Save Model
# -------------------------------
model_path = os.path.join(output_dir, "rf_model.joblib")
label_path = os.path.join(output_dir, "label_encoder.joblib")

joblib.dump(model, model_path)
joblib.dump(label_encoder, label_path)

print(f"\nModel saved to: {model_path}")
print(f"Label Encoder saved to: {label_path}")

# -------------------------------
# Visualization 1 – Actual vs Predicted
# -------------------------------
plt.figure(figsize=(10,6))
plt.scatter(y_test, y_pred)
plt.xlabel("Actual Execution Time (μs)")
plt.ylabel("Predicted Execution Time (μs)")
plt.title("Actual vs Predicted Execution Time")
plt.grid(True)

plot1_path = os.path.join(output_dir, "actual_vs_predicted.png")
plt.savefig(plot1_path)
plt.close()

print(f"Saved plot: {plot1_path}")

# -------------------------------
# Visualization 2 – Frequency of System Calls
# -------------------------------
plt.figure(figsize=(12,6))
df['syscall'].value_counts().plot(kind='bar')
plt.xlabel("System Call")
plt.ylabel("Frequency")
plt.title("System Call Frequency Distribution")

plot2_path = os.path.join(output_dir, "freq_dist.png")
plt.savefig(plot2_path)
plt.close()

print(f"Saved plot: {plot2_path}")

# -------------------------------
# Compute Slowest System Calls
# -------------------------------
latency_summary = df.groupby('syscall')['exec_time'].mean().sort_values(ascending=False)

summary_path = os.path.join(output_dir, "grouped_latency_summary.csv")
latency_summary.to_csv(summary_path)

print(f"Saved latency summary: {summary_path}")

# -------------------------------
# Generate Recommendations
# -------------------------------
recommendations_path = os.path.join(output_dir, "recommendations.txt")

with open(recommendations_path, "w") as f:
    f.write("=== AI-Generated Optimization Recommendations ===\n\n")
    f.write("Top 5 Slowest System Calls:\n")
    f.write(latency_summary.head(5).to_string())
    f.write("\n\n")

    f.write("Performance Recommendations:\n")
    f.write("- Reduce high-frequency system calls by batching operations.\n")
    f.write("- Minimize context switching where possible.\n")
    f.write("- Cache results for repetitive syscalls.\n")
    f.write("- Optimize file-system access patterns.\n")
    f.write("- Consider kernel-level syscall fusion for repeated patterns.\n")

print(f"\nSaved optimization recommendations: {recommendations_path}")

print("\n ALL TASKS COMPLETED SUCCESSFULLY ")
