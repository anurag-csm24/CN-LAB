"""
leaf_spine.py – Custom Leaf-Spine Topology for Assignment 14
"""

from mininet.topo import Topo
from mininet.net import Mininet
from mininet.cli import CLI
from mininet.log import setLogLevel

class LeafSpine(Topo):

    def build(self, spine_radix=2, leaf_radix=2, hosts_per_leaf=2):
        """
        spine_radix = number of spine switches
        leaf_radix  = number of leaf switches
        hosts_per_leaf = number of hosts per leaf switch
        """

        # Create spine switches
        spines = []
        for i in range(spine_radix):
            s = self.addSwitch(f'spine{i+1}')
            spines.append(s)

        # Create leaf switches and connect to spine switches
        for l in range(leaf_radix):
            leaf = self.addSwitch(f'leaf{l+1}')

            # Full mesh: connect leaf to all spine switches
            for spine in spines:
                self.addLink(leaf, spine)

            # Attach hosts to each leaf switch
            for h in range(hosts_per_leaf):
                host = self.addHost(f'h{l+1}{h+1}')
                self.addLink(host, leaf)


# Make the topology available
def run():
    topo = LeafSpine(spine_radix=2, leaf_radix=3, hosts_per_leaf=2)
    net = Mininet(topo=topo)
    net.start()
    print("Custom Leaf–Spine Topology Running...")
    CLI(net)
    net.stop()


if __name__ == '__main__':
    setLogLevel('info')
    run()

