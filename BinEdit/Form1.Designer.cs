namespace BinEdit
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            components = new System.ComponentModel.Container();
            TreeNode treeNode1 = new TreeNode("Root");
            treeView1 = new TreeView();
            CtxTreeStrip = new ContextMenuStrip(components);
            addToolStripMenuItem = new ToolStripMenuItem();
            renameToolStripMenuItem = new ToolStripMenuItem();
            panel1 = new Panel();
            label2 = new Label();
            comboBox1 = new ComboBox();
            label1 = new Label();
            textBox1 = new TextBox();
            button1 = new Button();
            button2 = new Button();
            button3 = new Button();
            button4 = new Button();
            SaveBin = new SaveFileDialog();
            OpenBin = new OpenFileDialog();
            removeToolStripMenuItem = new ToolStripMenuItem();
            CtxTreeStrip.SuspendLayout();
            panel1.SuspendLayout();
            SuspendLayout();
            // 
            // treeView1
            // 
            treeView1.BackColor = SystemColors.Control;
            treeView1.BorderStyle = BorderStyle.FixedSingle;
            treeView1.ContextMenuStrip = CtxTreeStrip;
            treeView1.Dock = DockStyle.Top;
            treeView1.LabelEdit = true;
            treeView1.Location = new Point(0, 0);
            treeView1.Name = "treeView1";
            treeNode1.Name = "Root";
            treeNode1.Text = "Root";
            treeView1.Nodes.AddRange(new TreeNode[] { treeNode1 });
            treeView1.Size = new Size(442, 435);
            treeView1.TabIndex = 0;
            treeView1.AfterLabelEdit += treeView1_AfterLabelEdit;
            treeView1.AfterSelect += treeView1_AfterSelect;
            // 
            // CtxTreeStrip
            // 
            CtxTreeStrip.Items.AddRange(new ToolStripItem[] { addToolStripMenuItem, renameToolStripMenuItem, removeToolStripMenuItem });
            CtxTreeStrip.Name = "CtxTreeStrip";
            CtxTreeStrip.Size = new Size(181, 92);
            CtxTreeStrip.ItemClicked += CtxTreeStrip_ItemClicked;
            // 
            // addToolStripMenuItem
            // 
            addToolStripMenuItem.Name = "addToolStripMenuItem";
            addToolStripMenuItem.Size = new Size(180, 22);
            addToolStripMenuItem.Text = "Add";
            // 
            // renameToolStripMenuItem
            // 
            renameToolStripMenuItem.Name = "renameToolStripMenuItem";
            renameToolStripMenuItem.Size = new Size(180, 22);
            renameToolStripMenuItem.Text = "Rename";
            // 
            // panel1
            // 
            panel1.BackColor = Color.Transparent;
            panel1.Controls.Add(label2);
            panel1.Controls.Add(comboBox1);
            panel1.Controls.Add(label1);
            panel1.Controls.Add(textBox1);
            panel1.Dock = DockStyle.Top;
            panel1.Location = new Point(0, 435);
            panel1.Name = "panel1";
            panel1.Size = new Size(442, 106);
            panel1.TabIndex = 1;
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(11, 9);
            label2.Name = "label2";
            label2.Size = new Size(34, 15);
            label2.TabIndex = 5;
            label2.Text = "Type:";
            // 
            // comboBox1
            // 
            comboBox1.FormattingEnabled = true;
            comboBox1.Items.AddRange(new object[] { "Unkown", "Int64", "Uint64", "Float", "Bool", "String" });
            comboBox1.Location = new Point(11, 27);
            comboBox1.Name = "comboBox1";
            comboBox1.Size = new Size(418, 23);
            comboBox1.TabIndex = 4;
            comboBox1.Text = "Unkown";
            comboBox1.SelectedIndexChanged += comboBox1_SelectedIndexChanged;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(11, 53);
            label1.Name = "label1";
            label1.Size = new Size(38, 15);
            label1.TabIndex = 3;
            label1.Text = "Value:";
            // 
            // textBox1
            // 
            textBox1.Location = new Point(11, 71);
            textBox1.Name = "textBox1";
            textBox1.Size = new Size(418, 23);
            textBox1.TabIndex = 2;
            textBox1.TextChanged += textBox1_TextChanged;
            // 
            // button1
            // 
            button1.Location = new Point(111, 547);
            button1.Name = "button1";
            button1.Size = new Size(75, 23);
            button1.TabIndex = 2;
            button1.Text = "New";
            button1.UseVisualStyleBackColor = true;
            button1.Click += button1_Click;
            // 
            // button2
            // 
            button2.Location = new Point(192, 547);
            button2.Name = "button2";
            button2.Size = new Size(75, 23);
            button2.TabIndex = 3;
            button2.Text = "Open";
            button2.UseVisualStyleBackColor = true;
            button2.Click += button2_Click;
            // 
            // button3
            // 
            button3.Location = new Point(273, 547);
            button3.Name = "button3";
            button3.Size = new Size(75, 23);
            button3.TabIndex = 4;
            button3.Text = "Save";
            button3.UseVisualStyleBackColor = true;
            button3.Click += button3_Click;
            // 
            // button4
            // 
            button4.Location = new Point(354, 547);
            button4.Name = "button4";
            button4.Size = new Size(75, 23);
            button4.TabIndex = 5;
            button4.Text = "Save as";
            button4.UseVisualStyleBackColor = true;
            button4.Click += button4_Click;
            // 
            // removeToolStripMenuItem
            // 
            removeToolStripMenuItem.Name = "removeToolStripMenuItem";
            removeToolStripMenuItem.Size = new Size(180, 22);
            removeToolStripMenuItem.Text = "Remove";
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(442, 577);
            Controls.Add(button4);
            Controls.Add(button3);
            Controls.Add(button2);
            Controls.Add(button1);
            Controls.Add(panel1);
            Controls.Add(treeView1);
            FormBorderStyle = FormBorderStyle.FixedSingle;
            MaximizeBox = false;
            Name = "Form1";
            SizeGripStyle = SizeGripStyle.Hide;
            Text = "BinEdit";
            CtxTreeStrip.ResumeLayout(false);
            panel1.ResumeLayout(false);
            panel1.PerformLayout();
            ResumeLayout(false);
        }

        #endregion

        private TreeView treeView1;
        private Panel panel1;
        private ContextMenuStrip CtxTreeStrip;
        private ToolStripMenuItem addToolStripMenuItem;
        private ToolStripMenuItem renameToolStripMenuItem;
        private Label label2;
        private ComboBox comboBox1;
        private Label label1;
        private TextBox textBox1;
        private Button button1;
        private Button button2;
        private Button button3;
        private Button button4;
        private SaveFileDialog SaveBin;
        private OpenFileDialog OpenBin;
        private ToolStripMenuItem removeToolStripMenuItem;
    }
}
