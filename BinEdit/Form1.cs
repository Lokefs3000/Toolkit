using System.Text;

namespace BinEdit
{
    public partial class Form1 : Form
    {
        private Dictionary<int, Node> Nodes = new Dictionary<int, Node>();
        private int NodeCounter = 1;
        private string SavedSaveLoc = string.Empty;

        public Form1()
        {
            InitializeComponent();

            Nodes.Add(0, new Node("Root"));
            treeView1.Nodes[0].Tag = 0;
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (e.Node != null && e.Node.Tag != null)
            {
                int tag = (int)treeView1.SelectedNode.Tag;
                Node node = Nodes[tag];

                comboBox1.SelectedIndex = (int)node.Type;
                textBox1.Text = node.Value == null ? "" : node.Value.ToString();
            }
        }

        private void CtxTreeStrip_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            if (e.ClickedItem != null)
            {
                if (e.ClickedItem.Text == "Add")
                {
                    int tag = (int)treeView1.SelectedNode.Tag;

                    Node node = Nodes[tag];

                    int id = NodeCounter++;
                    node.Nodes.Add(id);
                    Nodes.Add(id, new Node(NodeCounter.ToString()));

                    TreeNode treeNode = new TreeNode(NodeCounter.ToString());
                    treeNode.Tag = id;
                    treeView1.SelectedNode.Nodes.Add(treeNode);

                    Nodes[tag] = node;
                    treeView1.SelectedNode.Expand();
                }
                else if (e.ClickedItem.Text == "Rename")
                {
                    treeView1.SelectedNode.BeginEdit();
                }
                else if (e.ClickedItem.Text == "Remove")
                {
                    int tag = (int)treeView1.SelectedNode.Tag;

                    foreach (KeyValuePair<int, Node> kvp in Nodes)
                    {
                        if (kvp.Value.Nodes.Contains(tag))
                            kvp.Value.Nodes.Remove(tag);
                    }

                    RecursiveRemoveFromNode(treeView1.SelectedNode);

                    Nodes.Remove(tag);
                    treeView1.SelectedNode.Remove();
                }
            }
        }

        private void RecursiveRemoveFromNode(TreeNode node)
        {
            for (int i = 0; i < node.Nodes.Count; i++)
            {
                RecursiveRemoveFromNode(node.Nodes[i]);

                if (node.Nodes[i].Tag != null)
                {
                    int tag = (int)node.Nodes[i].Tag;
                    Nodes.Remove(tag);
                }
            }
        }

        private void treeView1_AfterLabelEdit(object sender, NodeLabelEditEventArgs e)
        {
            if (e.Node != null)
            {
                int tag = (int)e.Node.Tag;

                Node node = Nodes[tag];
                node.Name = e.Label != null ? e.Label : node.Name;

                Nodes[tag] = node;
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (treeView1.SelectedNode.Tag != null)
            {
                int tag = (int)treeView1.SelectedNode.Tag;

                Node node = Nodes[tag];
                node.Type = (NodeType)comboBox1.SelectedIndex;

                switch (node.Type)
                {
                    case NodeType.Int64:
                        node.Value = 0L;
                        break;
                    case NodeType.Uint64:
                        node.Value = 0UL;
                        break;
                    case NodeType.Float:
                        node.Value = 0.0f;
                        break;
                    case NodeType.String:
                        node.Value = "";
                        break;
                    case NodeType.Unkown:
                    default:
                        node.Value = null;
                        break;
                }

                textBox1.Text = (node.Value != null) ? node.Value.ToString() : string.Empty;

                Nodes[tag] = node;
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            if (treeView1.SelectedNode.Tag != null)
            {
                int tag = (int)treeView1.SelectedNode.Tag;

                Node node = Nodes[tag];

                switch (node.Type)
                {
                    case NodeType.Int64:
                        if (long.TryParse(textBox1.Text, out long resl))
                            node.Value = resl;
                        break;
                    case NodeType.Uint64:
                        if (ulong.TryParse(textBox1.Text, out ulong resul))
                            node.Value = resul;
                        break;
                    case NodeType.Float:
                        if (float.TryParse(textBox1.Text, out float resf))
                            node.Value = resf;
                        break;
                    case NodeType.String:
                        node.Value = textBox1.Text;
                        break;
                    case NodeType.Unkown:
                    default:
                        node.Value = null;
                        break;
                }

                Nodes[tag] = node;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            treeView1.Nodes.Clear();
            treeView1.Nodes.Add(new TreeNode("Root"));
            Nodes.Clear();
            Nodes.Add(0, new Node("Root"));
            treeView1.Nodes[0].Tag = 0;
            NodeCounter = 1;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            DialogResult res = OpenBin.ShowDialog();
            if (res == DialogResult.OK)
            {
                SavedSaveLoc = OpenBin.FileName;
                OpenFile(OpenBin.FileName);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (Path.Exists(SavedSaveLoc))
            {
                SaveFile(SavedSaveLoc);
            }
            else
            {
                button4_Click(sender, e);
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            DialogResult res = SaveBin.ShowDialog();
            if (res == DialogResult.OK)
            {
                SavedSaveLoc = SaveBin.FileName;
                SaveFile(SaveBin.FileName);
            }
        }

        private void OpenFile(string filename)
        {
            ushort version = 0;

            using (FileStream stream = File.OpenRead(filename))
            {
                using (BinaryReader br = new BinaryReader(stream))
                {
                    br.BaseStream.Position = 4;
                    version = br.ReadUInt16();
                }
            }

            switch (version)
            {
                case 1:
                    OpenFileV1(filename);
                    break;
                default:
                    break;
            }
        }

        private void OpenFileV1(string filename)
        {
            try
            {
                using (FileStream stream = File.OpenRead(filename))
                {
                    using (BinaryReader br = new BinaryReader(stream))
                    {
                        byte[] header = br.ReadBytes(4);
                        if (header[0] != 67 || header[1] != 66 || header[2] != 73 || header[3] != 78)
                            return;

                        treeView1.Nodes.Clear();
                        Nodes.Clear();

                        br.ReadUInt16(); //VERSION

                        int count = (int)br.ReadUInt32();

                        for (int i = 0; i < count; i++)
                        {
                            Node node = new Node();
                            node.Nodes = new List<int>();

                            int key = (int)br.ReadUInt32();

                            ushort nameLength = br.ReadUInt16();
                            byte[] nameBytes = br.ReadBytes(nameLength);
                            node.Name = Encoding.UTF8.GetString(nameBytes);

                            node.Type = (NodeType)br.ReadByte();

                            switch (node.Type)
                            {
                                case NodeType.Int64:
                                    node.Value = br.ReadInt64();
                                    break;
                                case NodeType.Uint64:
                                    node.Value = br.ReadUInt64();
                                    break;
                                case NodeType.Float:
                                    node.Value = br.ReadSingle();
                                    break;
                                case NodeType.Bool:
                                    node.Value = br.ReadBoolean();
                                    break;
                                case NodeType.String:
                                    ushort valueLength = br.ReadUInt16();
                                    byte[] valueBytes = br.ReadBytes(valueLength);
                                    node.Value = Encoding.UTF8.GetString(valueBytes);
                                    break;
                                case NodeType.Unkown:
                                default:
                                    break;
                            }

                            ushort childCount = br.ReadUInt16();

                            for (int j = 0; j < childCount; j++)
                            {
                                node.Nodes.Add((int)br.ReadUInt32());
                            }

                            Nodes.Add(key, node);
                        }

                        IterateFromRootNodeIndex(0, null);
                    }
                }
            }
            catch (Exception) { throw; }

        }

        private void IterateFromRootNodeIndex(int index, TreeNode? parent)
        {
            Node node = Nodes[index];

            TreeNode treeNode = new TreeNode(node.Name);
            treeNode.Tag = index;

            foreach (int child in node.Nodes)
            {
                IterateFromRootNodeIndex(child, treeNode);
            }

            if (parent == null)
                treeView1.Nodes.Add(treeNode);
            else
                parent.Nodes.Add(treeNode);
        }

        private void SaveFile(string filename)
        {
            try
            {
                using (FileStream stream = File.OpenWrite(filename))
                {
                    using (BinaryWriter bw = new BinaryWriter(stream))
                    {
                        bw.Write((byte)67); //HEADER 'C'
                        bw.Write((byte)66); //HEADER 'B'
                        bw.Write((byte)73); //HEADER 'I'
                        bw.Write((byte)78); //HEADER 'N'

                        bw.Write((ushort)1); //VERSION

                        bw.Write((uint)Nodes.Count); //COUNT

                        foreach (KeyValuePair<int, Node> node in Nodes)
                        {
                            bw.Write((uint)node.Key); //KEY / ID

                            bw.Write((ushort)node.Value.Name.Length); //NAME LENGTH
                            bw.Write(Encoding.UTF8.GetBytes(node.Value.Name)); //NAME

                            if (node.Value.Value != null)
                            {
                                bw.Write((byte)node.Value.Type); //DATA TYPE

                                switch (node.Value.Type)
                                {
                                    case NodeType.Int64:
                                        bw.Write((long)node.Value.Value); //VALUE
                                        break;
                                    case NodeType.Uint64:
                                        bw.Write((ulong)node.Value.Value); //VALUE
                                        break;
                                    case NodeType.Float:
                                        bw.Write((float)node.Value.Value); //VALUE
                                        break;
                                    case NodeType.Bool:
                                        bw.Write((bool)node.Value.Value); //VALUE
                                        break;
                                    case NodeType.String:
                                        string value = (string)node.Value.Value;
                                        bw.Write((ushort)value.Length); //VALUE LENGTH
                                        bw.Write(Encoding.UTF8.GetBytes(value)); //VALUE
                                        break;
                                    case NodeType.Unkown:
                                    default:
                                        break;
                                }
                            }
                            else
                                bw.Write((byte)NodeType.Unkown); //DATA TYPE

                            bw.Write((ushort)node.Value.Nodes.Count);

                            foreach (int child in node.Value.Nodes)
                            {
                                bw.Write((uint)child);
                            }
                        }
                    }
                }
            }
            catch (Exception) { };
        }

        public enum NodeType
        {
            Unkown,
            Int64,
            Uint64,
            Float,
            Bool,
            String
        }

        public struct Node
        {
            public NodeType Type = NodeType.Unkown;

            public string Name = "";
            public object? Value = null;

            public List<int> Nodes = new List<int>();

            public Node(string name)
            {
                Name = name;
                Nodes = new List<int>();
            }
        }
    }
}
