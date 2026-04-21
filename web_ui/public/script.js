const textarea = document.getElementById('command-input');
const lineNumbers = document.getElementById('line-numbers');
const btnExample = document.getElementById('btn-example');
const btnRun = document.getElementById('btn-run');
const terminalOutput = document.getElementById('terminal-output');

function updateLineNumbers() {
    const lines = textarea.value.split('\n').length;
    lineNumbers.innerHTML = Array(lines).fill(0).map((_, i) => i + 1).join('<br>');
}

textarea.addEventListener('input', updateLineNumbers);
textarea.addEventListener('scroll', () => {
    lineNumbers.scrollTop = textarea.scrollTop;
});

// Initial update
updateLineNumbers();

btnExample.addEventListener('click', () => {
    textarea.value = `GCD 12 18\nGCD -5 10\nPOW 2 3 5\nPRIME 7\nINV 3 11\nPHI 10\nCHECK 3 11`;
    updateLineNumbers();
});

btnRun.addEventListener('click', async () => {
    const rawInput = textarea.value;
    const commands = rawInput.split('\n').map(line => line.trim()).filter(line => line.length > 0 && !line.startsWith('#'));
    
    if (commands.length === 0) {
        terminalOutput.innerHTML = '<span class="error">Error: Please enter some commands first.</span>';
        return;
    }

    terminalOutput.innerHTML = 'Executing...';
    btnRun.disabled = true;
    btnRun.innerHTML = 'Running... <span class="glow"></span>';

    try {
        const response = await fetch('/api/execute', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({ commands })
        });

        const data = await response.json();
        
        if (!response.ok) {
            terminalOutput.innerHTML = `<span class="error">Failed: ${data.error}</span>\n\n${data.details || ''}`;
        } else {
            // Format the output
            let formattedOutput = '';
            data.results.forEach(res => {
                if (res.includes('ERROR_') || res.includes('FAILED')) {
                    formattedOutput += `<span class="error">${res}</span>\n`;
                } else {
                    formattedOutput += `<span class="success">${res}</span>\n`;
                }
            });
            terminalOutput.innerHTML = formattedOutput;
        }
    } catch (error) {
        terminalOutput.innerHTML = `<span class="error">Network Error: Could not connect to the execution server.</span>`;
    } finally {
        btnRun.disabled = false;
        btnRun.innerHTML = 'Run Execution <span class="glow"></span>';
    }
});
