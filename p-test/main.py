from scipy.stats import binomtest

# Data from your 4 questions (Total N=7 for all)
# successes = number of "Agree" + "Strongly Agree" answers combined
experiments = {
    "Q1 (New build orders)": {"successes": 3, "n": 7}, # 3 Positive, 4 Negative
    "Q2 (New strategies)": {"successes": 5, "n": 7},   # 5 Positive, 2 Negative
    "Q3 (Replay tool)": {"successes": 5, "n": 7},      # 5 Positive, 2 Negative
    "Q4 (New maneuvers)": {"successes": 4, "n": 7}     # 4 Positive, 3 Negative
}

print("P-values for your 4 thesis questions:\n")

for name, data in experiments.items():
    # We test against a 0.5 (50/50) probability null hypothesis
    result = binomtest(data["successes"], data["n"], p=0.5, alternative='two-sided')
    print(f"{name}: p-value = {result.pvalue:.4f}")
