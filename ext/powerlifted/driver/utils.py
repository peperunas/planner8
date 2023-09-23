import os

from pathlib import Path

def get_elapsed_time():
    """
    Return the CPU time taken by the python process and its child
    processes.

    From Fast Downward source code.
    """
    if os.name == "nt":
        # The child time components of os.times() are 0 on Windows.
        raise NotImplementedError("cannot use get_elapsed_time() on Windows")
    return sum(os.times()[:4])


def find_domain_filename(task_filename):
    """
    Find domain filename for the given task using automatic naming rules.
    """
    dirname, basename = os.path.split(task_filename)

    domain_basenames = [
        "domain.pddl",
        basename[:3] + "-domain.pddl",
        "domain_" + basename,
        "domain-" + basename,
    ]

    for domain_basename in domain_basenames:
        domain_filename = os.path.join(dirname, domain_basename)
        if os.path.exists(domain_filename):
            return domain_filename


def remove_temporary_files(options):
    files: list[Path] = list(
        map(
            lambda x: Path(x),
            [options.translator_file, "new-instance-file.pddl", "new-domain-file.pddl"],
        )
    )

    for f in files:
        f.unlink(missing_ok=True)
