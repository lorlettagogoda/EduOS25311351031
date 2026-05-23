import csv
import random
import matplotlib.pyplot as plt

# =========================================================
# LOAD PROCESSES FROM CSV
# =========================================================

def load_processes(file_path):

    processes = []

    with open(file_path, 'r') as file:

        reader = csv.DictReader(file)

        for row in reader:

            processes.append({
                "pid": int(row["pid"]),
                "arrival_time": int(row["arrival_time"]),
                "burst_time": int(row["burst_time"]),
                "priority": int(row["priority"])
            })

    return processes

# =========================================================
# GENERATE RANDOM PROCESSES
# =========================================================

def generate_random_processes(n, seed):

    random.seed(seed)

    processes = []

    for i in range(1, n + 1):

        processes.append({
            "pid": i,
            "arrival_time": random.randint(0, 5),
            "burst_time": random.randint(1, 10),
            "priority": random.randint(1, 5)
        })

    return processes

# =========================================================
# FCFS SCHEDULING
# =========================================================

def fcfs(processes):

    processes.sort(key=lambda x: (x["arrival_time"], x["pid"]))

    current_time = 0

    schedule = []

    results = []

    for p in processes:

        if current_time < p["arrival_time"]:
            current_time = p["arrival_time"]

        start = current_time

        end = start + p["burst_time"]

        turnaround = end - p["arrival_time"]

        waiting = turnaround - p["burst_time"]

        response = waiting

        schedule.append((p["pid"], start, end))

        results.append({
            "PID": p["pid"],
            "AT": p["arrival_time"],
            "BT": p["burst_time"],
            "CT": end,
            "TAT": turnaround,
            "WT": waiting,
            "RT": response
        })

        current_time = end

    return schedule, results

# =========================================================
# SJF SCHEDULING
# =========================================================

def sjf(processes):

    processes.sort(key=lambda x: x["arrival_time"])

    current_time = 0

    completed = []

    ready_queue = []

    schedule = []

    while len(completed) < len(processes):

        for p in processes:

            if p not in completed and p not in ready_queue:

                if p["arrival_time"] <= current_time:

                    ready_queue.append(p)

        if len(ready_queue) == 0:

            current_time += 1

            continue

        ready_queue.sort(key=lambda x: (x["burst_time"], x["arrival_time"]))

        p = ready_queue.pop(0)

        start = current_time

        end = start + p["burst_time"]

        turnaround = end - p["arrival_time"]

        waiting = turnaround - p["burst_time"]

        response = waiting

        schedule.append((p["pid"], start, end))

        completed.append(p)

        completed_result = {
            "PID": p["pid"],
            "AT": p["arrival_time"],
            "BT": p["burst_time"],
            "CT": end,
            "TAT": turnaround,
            "WT": waiting,
            "RT": response
        }

        current_time = end

    results = []

    for p in completed:

        completion_time = 0

        for item in schedule:

            if item[0] == p["pid"]:
                completion_time = item[2]

        turnaround = completion_time - p["arrival_time"]

        waiting = turnaround - p["burst_time"]

        response = waiting

        results.append({
            "PID": p["pid"],
            "AT": p["arrival_time"],
            "BT": p["burst_time"],
            "CT": completion_time,
            "TAT": turnaround,
            "WT": waiting,
            "RT": response
        })

    return schedule, results

# =========================================================
# PRIORITY SCHEDULING
# =========================================================

def priority_scheduling(processes):

    processes.sort(key=lambda x: x["arrival_time"])

    current_time = 0

    completed = []

    ready_queue = []

    schedule = []

    results = []

    while len(completed) < len(processes):

        for p in processes:

            if p not in completed and p not in ready_queue:

                if p["arrival_time"] <= current_time:

                    ready_queue.append(p)

        if len(ready_queue) == 0:

            current_time += 1

            continue

        # AGEING

        for p in ready_queue:

            waiting_time = current_time - p["arrival_time"]

            if waiting_time > 0 and waiting_time % 3 == 0:

                p["priority"] = max(0, p["priority"] - 1)

        ready_queue.sort(key=lambda x: (x["priority"], x["arrival_time"]))

        p = ready_queue.pop(0)

        start = current_time

        end = start + p["burst_time"]

        turnaround = end - p["arrival_time"]

        waiting = turnaround - p["burst_time"]

        response = waiting

        schedule.append((p["pid"], start, end))

        completed.append(p)

        results.append({
            "PID": p["pid"],
            "AT": p["arrival_time"],
            "BT": p["burst_time"],
            "PR": p["priority"],
            "CT": end,
            "TAT": turnaround,
            "WT": waiting,
            "RT": response
        })

        current_time = end

    return schedule, results

# =========================================================
# ROUND ROBIN
# =========================================================

def round_robin(processes, quantum):

    processes.sort(key=lambda x: x["arrival_time"])

    queue = []

    remaining = {}

    current_time = 0

    schedule = []

    results = []

    completed = []

    for p in processes:

        remaining[p["pid"]] = p["burst_time"]

    i = 0

    while len(completed) < len(processes):

        while i < len(processes) and processes[i]["arrival_time"] <= current_time:

            queue.append(processes[i])

            i += 1

        if len(queue) == 0:

            current_time += 1

            continue

        p = queue.pop(0)

        start = current_time

        execution = min(quantum, remaining[p["pid"]])

        current_time += execution

        end = current_time

        remaining[p["pid"]] -= execution

        schedule.append((p["pid"], start, end))

        while i < len(processes) and processes[i]["arrival_time"] <= current_time:

            queue.append(processes[i])

            i += 1

        if remaining[p["pid"]] > 0:

            queue.append(p)

        else:

            completed.append(p["pid"])

            turnaround = current_time - p["arrival_time"]

            waiting = turnaround - p["burst_time"]

            response = waiting

            results.append({
                "PID": p["pid"],
                "AT": p["arrival_time"],
                "BT": p["burst_time"],
                "CT": current_time,
                "TAT": turnaround,
                "WT": waiting,
                "RT": response
            })

    return schedule, results

# =========================================================
# PRINT RESULTS TABLE
# =========================================================

def print_results(results):

    print("\n======================================================")
    print("PID\tAT\tBT\tCT\tTAT\tWT\tRT")
    print("======================================================")

    for r in results:

        print(
            f"{r['PID']}\t"
            f"{r['AT']}\t"
            f"{r['BT']}\t"
            f"{r['CT']}\t"
            f"{r['TAT']}\t"
            f"{r['WT']}\t"
            f"{r['RT']}"
        )

# =========================================================
# CALCULATE METRICS
# =========================================================

def calculate_metrics(results):

    n = len(results)

    total_wt = 0
    total_tat = 0
    total_rt = 0
    total_bt = 0
    total_ct = 0

    for r in results:

        total_wt += r["WT"]
        total_tat += r["TAT"]
        total_rt += r["RT"]
        total_bt += r["BT"]

        if r["CT"] > total_ct:
            total_ct = r["CT"]

    avg_wt = total_wt / n

    avg_tat = total_tat / n

    avg_rt = total_rt / n

    cpu_util = (total_bt / total_ct) * 100

    throughput = n / total_ct

    print("\n==============================")
    print("AVERAGE METRICS")
    print("==============================")

    print(f"Average WT: {avg_wt:.2f}")
    print(f"Average TAT: {avg_tat:.2f}")
    print(f"Average RT: {avg_rt:.2f}")
    print(f"CPU Utilisation: {cpu_util:.2f}%")
    print(f"Throughput: {throughput:.2f}")

# =========================================================
# DRAW GANTT CHART
# =========================================================

def draw_gantt(schedule, title):

    fig, ax = plt.subplots(figsize=(10, 5))

    for task in schedule:

        pid = task[0]
        start = task[1]
        end = task[2]

        ax.barh(
            y=f"P{pid}",
            width=end - start,
            left=start,
            height=0.5
        )

        ax.text(
            start + ((end - start) / 2),
            f"P{pid}",
            f"P{pid}",
            ha='center',
            va='center',
            color='white'
        )

    ax.set_xlabel("Time")

    ax.set_title(title)

    plt.savefig(f"{title}.png")

    plt.show()

# =========================================================
# MAIN PROGRAM
# =========================================================

print("===================================")
print("EDUOS PROCESS SCHEDULER")
print("===================================")

print("\nChoose Input Method")
print("1. Load CSV File")
print("2. Generate Random Processes")

choice = input("Enter choice: ")

if choice == "1":

    filename = input("Enter CSV filename: ")

    processes = load_processes(filename)

elif choice == "2":

    n = int(input("Number of processes: "))

    seed = int(input("Enter seed value: "))

    processes = generate_random_processes(n, seed)

else:

    print("Invalid Choice")

    exit()

print("\nChoose Algorithm")
print("1. FCFS")
print("2. SJF")
print("3. Priority")
print("4. Round Robin")

algorithm = input("Enter choice: ")

if algorithm == "1":

    schedule, results = fcfs(processes)

    title = "FCFS"

elif algorithm == "2":

    schedule, results = sjf(processes)

    title = "SJF"

elif algorithm == "3":

    schedule, results = priority_scheduling(processes)

    title = "PRIORITY"

elif algorithm == "4":

    quantum = int(input("Enter Quantum: "))

    schedule, results = round_robin(processes, quantum)

    title = "ROUND_ROBIN"

else:

    print("Invalid Algorithm")

    exit()

# =========================================================
# DISPLAY OUTPUT
# =========================================================

print_results(results)

calculate_metrics(results)

draw_gantt(schedule, title)