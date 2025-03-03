int 
clone(void (*fn)(void *), void *arg, void *stack) { 
  struct proc *np;
  struct proc *curproc = myproc();

  if (!stack || (uint)stack % PGSIZE != 0) {  // Ensure stack is page-aligned
      return -1;
  }

  np = allocproc();
  if (!np) {
      return -1;
  }

  // Create a copy of the page table
  np->pgdir = copyuvm(curproc->pgdir, curproc->sz);
  if (!np->pgdir) {
      kfree(np->kstack);
      np->state = UNUSED;
      return -1;
  }

  // Share heap pages explicitly
  uint heap_start = curproc->sz; // Heap starts at curproc->sz
  for (uint va = heap_start; va < curproc->sz; va += PGSIZE) {
      pte_t *pte = walkpgdir(curproc->pgdir, (void *)va, 0);
      if (pte && (*pte & PTE_P)) {
          mappages(np->pgdir, (void *)va, PGSIZE, PTE_ADDR(*pte), PTE_W | PTE_U);
      }
  }

  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;  // Copy trap frame

  // Set up new stack
  np->tf->esp = (uint)stack + PGSIZE - 4;
  *(uint *)(np->tf->esp) = (uint)arg; // Push argument
  np->tf->esp -= 4;
  *(uint *)(np->tf->esp) = 0; // Fake return address
  np->tf->eip = (uint)fn;  // Set function entry point

  // Copy file descriptors
  for (int i = 0; i < NOFILE; i++) {
      if (curproc->ofile[i]) {
          np->ofile[i] = filedup(curproc->ofile[i]);
      }
  }

  np->cwd = idup(curproc->cwd);
  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  np->state = RUNNABLE;
  return np->pid;
}
