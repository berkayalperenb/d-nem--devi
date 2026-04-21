const express = require('express');
const cors = require('cors');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

const app = express();
app.use(cors());
app.use(express.json());
app.use(express.static('public'));

const exePath = path.join(__dirname, '..', 'numtool.exe');

app.post('/api/execute', (req, res) => {
    const { commands } = req.body;
    if (!commands || !Array.isArray(commands)) {
        return res.status(400).json({ error: 'Invalid commands format' });
    }

    const uniqueId = crypto.randomUUID();
    const inputPath = path.join(__dirname, '..', `input_${uniqueId}.txt`);
    const outputPath = path.join(__dirname, '..', `output_${uniqueId}.txt`);

    // Write commands to input file
    fs.writeFileSync(inputPath, commands.join('\n') + '\n');

    // Execute the compiled program
    exec(`"${exePath}" "${inputPath}" "${outputPath}"`, (error, stdout, stderr) => {
        let resultData = "";
        try {
            if (fs.existsSync(outputPath)) {
                resultData = fs.readFileSync(outputPath, 'utf8');
            }
        } catch (e) {
            console.error("Error reading output", e);
        }

        // Clean up temporary files
        try { if (fs.existsSync(inputPath)) fs.unlinkSync(inputPath); } catch(e){}
        try { if (fs.existsSync(outputPath)) fs.unlinkSync(outputPath); } catch(e){}

        if (error) {
            console.error(`Execution error: ${error.message}`);
            return res.status(500).json({ error: 'Execution failed', details: stderr || error.message, results: resultData.trim().split('\n') });
        }

        res.json({ results: resultData.trim().split('\n').filter(l => l) });
    });
});

const PORT = 3000;
app.listen(PORT, () => {
    console.log(`UI Server running at http://localhost:${PORT}`);
});
