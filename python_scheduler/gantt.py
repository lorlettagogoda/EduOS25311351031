import matplotlib.pyplot as plt
import matplotlib.patches as mpatches

# =========================================================
# ADVANCED GANTT CHART FOR EDUOS ASSIGNMENT
# =========================================================

def draw_gantt(schedule, title):

    """
    schedule format:
    [
        (pid, start_time, end_time),
        (pid, start_time, end_time)
    ]
    """

    fig, ax = plt.subplots(figsize=(15, 6))

    # COLORS FOR DIFFERENT PROCESSES

    colors = [
        "blue",
        "green",
        "orange",
        "red",
        "purple",
        "brown",
        "pink",
        "gray",
        "olive",
        "cyan"
    ]

    process_colors = {}

    y_position = 10

    process_count = 0

    legend_handles = []

    # =====================================================
    # DRAW EACH PROCESS
    # =====================================================

    for task in schedule:

        pid = task[0]
        start = task[1]
        end = task[2]

        duration = end - start

        # ASSIGN UNIQUE COLOR

        if pid not in process_colors:

            process_colors[pid] = colors[process_count % len(colors)]

            process_count += 1

        color = process_colors[pid]

        # DRAW PROCESS BAR

        ax.broken_barh(
            [(start, duration)],
            (y_position, 8),
            facecolors=color
        )

        # WRITE PROCESS LABEL

        ax.text(
            start + duration / 2,
            y_position + 4,
            f"P{pid}",
            ha='center',
            va='center',
            color='white',
            fontsize=10,
            fontweight='bold'
        )

        # SHOW START TIME

        ax.text(
            start,
            y_position - 2,
            str(start),
            fontsize=9
        )

        # SHOW END TIME

        ax.text(
            end,
            y_position - 2,
            str(end),
            fontsize=9
        )

    # =====================================================
    # X AXIS SETTINGS
    # =====================================================

    max_time = max(task[2] for task in schedule)

    ax.set_xlim(0, max_time + 2)

    ax.set_xticks(range(0, max_time + 2))

    # =====================================================
    # Y AXIS SETTINGS
    # =====================================================

    ax.set_ylim(5, 25)

    ax.set_yticks([14])

    ax.set_yticklabels(["CPU"])

    # =====================================================
    # LABELS AND TITLE
    # =====================================================

    ax.set_xlabel("TIME")

    ax.set_ylabel("PROCESS EXECUTION")

    ax.set_title(title + " GANTT CHART")

    # =====================================================
    # GRID
    # =====================================================

    ax.grid(True)

    # =====================================================
    # CREATE LEGEND
    # =====================================================

    for pid in process_colors:

        patch = mpatches.Patch(
            color=process_colors[pid],
            label=f"P{pid}"
        )

        legend_handles.append(patch)

    ax.legend(
        handles=legend_handles,
        loc='upper right'
    )

    # =====================================================
    # SAVE IMAGE
    # =====================================================

    filename = title.lower() + "_gantt_chart.png"

    plt.savefig(filename)

    print("\nGantt chart saved as:", filename)

    # =====================================================
    # SHOW CHART
    # =====================================================

    plt.show()

# =========================================================
# SAMPLE DATA
# =========================================================

schedule = [
    (1, 0, 4),
    (2, 4, 7),
    (3, 7, 12),
    (4, 12, 15),
    (2, 15, 18)
]

# =========================================================
# CALL FUNCTION
# =========================================================

draw_gantt(schedule, "ROUND ROBIN")