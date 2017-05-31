import time

def findNearest(root, key):
    v = root
    while v.key != key:
        if v.key > key:
            if v.left is None:
                return v
            v = v.left
        else:
            if v.right is None:
                return v
            v = v.right
    return v

class Node:
    def __init__(self, key):
        self.key = key
        self.sum = key
        self.parent = None
        self.left = None
        self.right = None

    def setLeft(self, left):
        ds = 0 if self.left is None else self.left.sum
        self.left = left
        ds -= 0 if left is None else left.sum
        self.sum -= ds


    def setRight(self, right):
        ds = 0 if self.right is None else self.right.sum
        self.right = right
        ds -= 0 if right is None else right.sum
        self.sum -= ds

class SplayTree:
    def __init__(self):
        self.root = None
        self.findCount = 0

    def insert(self, key):
        if (self.root == None):
            self.root = Node(key)
            return
        
        nearestNode = findNearest(self.root, key)
        if nearestNode.key == key:
            return

        self.splay(nearestNode)
        
        n = Node(key)
        if key < nearestNode.key:
            ds = 0
            n.left = nearestNode.left
            if n.left is not None:
                n.left.parent = n
                ss = nearestNode.left.sum
                ds -= ss
                nearestNode.sum -= ss
            n.sum -= ds
            nearestNode.left = None
            n.right = nearestNode
            n.sum += nearestNode.sum
            nearestNode.parent = n
        else:
            ds = 0 if n.right is None else n.right.sum
            n.right = nearestNode.right
            ds -= 0 if nearestNode.right is None else nearestNode.right.sum
            n.sum -= ds
            
            if n.right is not None:
                n.right.parent = n
            if nearestNode.right is not None:
                nearestNode.sum -= nearestNode.right.sum
            nearestNode.right = None
        
            ds = 0 if n.left is None else n.left.sum
            n.left = nearestNode
            ds -= 0 if nearestNode is None else nearestNode.sum
            n.sum -= ds
            
            nearestNode.parent = n    
        self.root = n


    def remove(self, key):
        if self.root is None:
            return
        nearestNode = findNearest(self.root, key)
        if nearestNode.key != key:
            return

        self.splay(nearestNode)

        if nearestNode.left == None:
            self.root = nearestNode.right
            if nearestNode.right is not None:
                nearestNode.right.parent = None
        else:
            x = nearestNode.right
            nearestNode.left.parent = None
            self.root = nearestNode.left
            
            self.splay(findNearest(self.root, key))
            
            ds = 0 if self.root.right is None else self.root.right.sum
            self.root.right = x
            ds -= 0 if x is None else x.sum
            self.root.sum -= ds
            
            if x is not None:
                x.parent = self.root

    def find(self, key):
        if self.root is None:
            return False
        nearestNode = findNearest(self.root, key)
        self.findCount += 1
        if self.findCount == 100:
            self.findCount = 0
            self.splay(nearestNode)
        return nearestNode.key == key

    def getSum(self, left, right):
        v = self.root
        while v is not None:
            if right < v.key:
                v = v.left
            elif left > v.key:
                v = v.right
            else:
                break
        if v is None:
            return 0
        
        # do some magic
        self.splay(v)
        
        s = v.sum
        nLeft = v.left
        while nLeft is not None:
            if left < nLeft.key:
                nLeft = nLeft.left
            elif left > nLeft.key:
                s -= nLeft.key
                if nLeft.left is not None:
                    s -= nLeft.left.sum
                nLeft = nLeft.right
            else:
                if nLeft.left is not None:
                    s -= nLeft.left.sum
                break
        nRight = v.right
        while nRight is not None:
            if right > nRight.key:
                nRight = nRight.right
            else:
                if nRight.right is not None:
                    s -= nRight.right.sum
                if right < nRight.key:
                    s -= nRight.key
                    nRight = nRight.left
                else:
                    break
        
        return s

    def splay(self, v):
        self.findCount = 0
        while v.parent is not None:
            parent = v.parent
            gparent = parent.parent
            if gparent is None:
                # SplayTree.__rotate(parent, v)
                if parent.left == v:
                    #parent.setLeft(v.right)
                    ds = 0 if parent.left is None else parent.left.sum
                    parent.left = v.right
                    ds -= 0 if v.right is None else v.right.sum
                    parent.sum -= ds
                    
                    if parent.left is not None:
                        parent.left.parent = parent
                    #v.setRight(parent)
                    ds = 0 if v.right is None else v.right.sum
                    v.right = parent
                    ds -= 0 if parent is None else parent.sum
                    v.sum -= ds
                    
                    parent.parent = v
                else:
                    parent.setRight(v.left)
                    if parent.right is not None:
                        parent.right.parent = parent
                    v.setLeft(parent)
                    parent.parent = v
                v.parent = None
                break
            if (gparent.left == parent) == (parent.left == v):
                # SplayTree.__rotate(gparent, parent)
                gparent_ = gparent.parent
                if gparent.left == parent:
                    gparent.setLeft(parent.right)
                    if gparent.left is not None:
                        gparent.left.parent = gparent
                    parent.setRight(gparent)
                    gparent.parent = parent
                else:
                    gparent.setRight(parent.left)
                    if gparent.right is not None:
                        gparent.right.parent = gparent
                    parent.setLeft(gparent)
                    gparent.parent = parent

                parent.parent = gparent_
                if gparent_ is not None:
                    if gparent_.left == gparent:
                        gparent_.left = parent
                    else:
                        gparent_.right = parent
                # SplayTree.__rotate(parent, v)
                gparent = parent.parent
                if parent.left == v:
                    parent.setLeft(v.right)
                    if parent.left is not None:
                        parent.left.parent = parent
                    v.setRight(parent)
                    parent.parent = v
                else:
                    parent.setRight(v.left)
                    if parent.right is not None:
                        parent.right.parent = parent
                    v.setLeft(parent)
                    parent.parent = v

                v.parent = gparent
                if gparent is not None:
                    if gparent.left == parent:
                        gparent.left = v
                    else:
                        gparent.right = v
            else:
                # SplayTree.__rotate(parent, v)
                # gparent = parent.parent
                if parent.left == v:
                    parent.setLeft(v.right)
                    if parent.left is not None:
                        parent.left.parent = parent
                    v.setRight(parent)
                    parent.parent = v
                else:
                    parent.setRight(v.left)
                    if parent.right is not None:
                        parent.right.parent = parent
                    v.setLeft(parent)
                    parent.parent = v

                v.parent = gparent
                if gparent is not None:
                    if gparent.left == parent:
                        gparent.left = v
                    else:
                        gparent.right = v
                # SplayTree.__rotate(gparent, v)
                gparent_ = gparent.parent
                if gparent.left == v:
                    gparent.setLeft(v.right)
                    if gparent.left is not None:
                        gparent.left.parent = gparent
                    v.setRight(gparent)
                    gparent.parent = v
                else:
                    gparent.setRight(v.left)
                    if gparent.right is not None:
                        gparent.right.parent = gparent
                    v.setLeft(gparent)
                    gparent.parent = v

                v.parent = gparent_
                if gparent_ is not None:
                    if gparent_.left == gparent:
                        gparent_.left = v
                    else:
                        gparent_.right = v

        self.root = v
        return v


start = time.time()

n = int(input())
tree = SplayTree()
s = 0
for _ in range(n):
    inStr = input()
    cmd = inStr.split()
    if cmd[0] == '+':
        v = (int(cmd[1]) + s) % 1000000001
        tree.insert(v)
    elif cmd[0] == '-':
        v = (int(cmd[1]) + s) % 1000000001
        tree.remove(v)
    elif cmd[0] == '?':
        v = (int(cmd[1]) + s) % 1000000001
        if tree.find(v):
            print('Found')
        else:
            print('Not found')
    elif cmd[0] == 's':
        if tree.root is not None:
            fl = (int(cmd[1]) + s) % 1000000001
            fr = (int(cmd[2]) + s) % 1000000001
            s = tree.getSum(fl, fr)
        else:
            s = 0
        print(s)

print("Time elapsed for:", time.time()-start)
