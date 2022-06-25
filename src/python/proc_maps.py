#!/usr/bin/python3

import logging
import re
from dataclasses import dataclass

MAPS_LINE_RE = re.compile(r"""
    (?P<addr_start>[0-9a-f]+)-(?P<addr_end>[0-9a-f]+)\s+  # Address
    (?P<perms>\S+)\s+                                     # Permissions
    (?P<offset>[0-9a-f]+)\s+                              # Map offset
    (?P<dev>\S+)\s+                                       # Device node
    (?P<inode>\d+)\s+                                     # Inode
    (?P<pathname>.*)\s+                                   # Pathname
""", re.VERBOSE)

def human_bytes(size):
    modifier = 1
    while size > 1024:
        modifier *= 1024
        size /= 1024
    return "%.1f%s" % (size, {
        1024**0: 'b',
        1024**1: 'k',
        1024**2: 'M',
        1024**3: 'G',
        1024**4: 'T',
    }.get(modifier, " x%d" % modifier))

@dataclass
class proc_maps_record:
    addr_start: int
    addr_end: int
    perms: str
    offset: int
    dev: str
    inode: int
    pathname: str


    @property
    def size(self) -> int:
        return self.addr_end - self.addr_start

    @property
    def human_size(self) -> int:
        return human_bytes(self.size)

    @property
    def readable(self) -> bool:
        return self.perms[0] == "r"

    @property
    def writable(self) -> bool:
        return self.perms[1] == "w"

    @property
    def executable(self) -> bool:
        return self.perms[2] == "x"

    @property
    def shared(self) -> bool:
        return self.perms[3] == "s"

    @property
    def private(self) -> bool:
        return self.perms[3] == "p"

    @classmethod
    def parse(self, pid):
        records = []
        with open("/proc/%d/maps" % pid) as fd:
            for line in fd:
                m = MAPS_LINE_RE.match(line)
                if not m:
                    continue
                addr_start, addr_end, perms, offset, dev, inode, pathname = m.groups()
                addr_start = int(addr_start, 16)
                addr_end = int(addr_end, 16)
                offset = int(offset, 16)
                records.append(proc_maps_record(
                    addr_start=addr_start,
                    addr_end=addr_end,
                    perms=perms,
                    offset=offset,
                    dev=dev,
                    inode=inode,
                    pathname=pathname,
                ))
        return records

def get_text_section_record(pid):
    records = proc_maps_record.parse(pid)
    first = True
    executable_name = ""

    if not records:
        logging.error(f'Empty /proc/{pid}/pid file or it could not be parser')
        raise Exception(f'Could not parse /proc/{pid}/maps file')
    for record in records:
        if first:
            executable_name = record.pathname
            first = False

        if record.readable and record.executable and record.private and record.pathname == executable_name:
            logging.info(f'Found text section record for executable: {executable_name}')
            return record
    
    logging.error(f'Could not find text section for {executable_name}')
    raise Exception(f'Could not found text section for {executable_name}')
