namespace ConnectToRaspberry
{
    partial class Form1
    {
        /// <summary>
        /// Variable nécessaire au concepteur.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Nettoyage des ressources utilisées.
        /// </summary>
        /// <param name="disposing">true si les ressources managées doivent être supprimées ; sinon, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Code généré par le Concepteur Windows Form

        /// <summary>
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InitializeComponent()
        {
            this.connection_button = new System.Windows.Forms.Button();
            this.disconnection_button = new System.Windows.Forms.Button();
            this.IPList = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // connection_button
            // 
            this.connection_button.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F);
            this.connection_button.Location = new System.Drawing.Point(12, 40);
            this.connection_button.Name = "connection_button";
            this.connection_button.Size = new System.Drawing.Size(352, 27);
            this.connection_button.TabIndex = 0;
            this.connection_button.Text = "Connection to Raspberry";
            this.connection_button.UseVisualStyleBackColor = true;
            this.connection_button.Click += new System.EventHandler(this.connection_button_Click);
            // 
            // disconnection_button
            // 
            this.disconnection_button.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F);
            this.disconnection_button.Location = new System.Drawing.Point(12, 73);
            this.disconnection_button.Name = "disconnection_button";
            this.disconnection_button.Size = new System.Drawing.Size(352, 27);
            this.disconnection_button.TabIndex = 1;
            this.disconnection_button.Text = "Disconnection from Raspberry";
            this.disconnection_button.UseVisualStyleBackColor = true;
            this.disconnection_button.Click += new System.EventHandler(this.disconnection_button_Click);
            // 
            // IPList
            // 
            this.IPList.FormattingEnabled = true;
            this.IPList.Location = new System.Drawing.Point(12, 13);
            this.IPList.Name = "IPList";
            this.IPList.Size = new System.Drawing.Size(352, 21);
            this.IPList.TabIndex = 2;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(376, 111);
            this.Controls.Add(this.IPList);
            this.Controls.Add(this.disconnection_button);
            this.Controls.Add(this.connection_button);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button connection_button;
        private System.Windows.Forms.Button disconnection_button;
        private System.Windows.Forms.ComboBox IPList;
    }
}

