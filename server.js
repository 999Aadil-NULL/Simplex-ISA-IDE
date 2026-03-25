const express = require('express');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');
const cors = require('cors');
const crypto = require('crypto'); 

const app = express();
app.set('view engine', 'ejs');
app.use(cors());
app.use(express.json());

app.get('/', (req, res) => {
    res.render('index', {
        pageTitle: 'SIMPLEX IDE',
        initialCode: '; SIMPLEX MIPS Program\n; Code starts at address zero\n HALT\n',
    });
});

app.post('/assemble', (req, res) => {
    const code = req.body.code;
    

    const id = crypto.randomBytes(4).toString('hex');
    const filenames = {
        asm: `main_${id}.asm`,
        obj: `main_${id}.obj`,
        log: `main_${id}.log`,
        lst: `main_${id}.lst`,
        trace: `main_${id}.trace`
    };


    fs.writeFileSync(filenames.asm, code);

    const isWin = process.platform === "win32";

    const asmCmd = isWin ? `asm.exe ${filenames.asm}` : `./asm ${filenames.asm}`;
    const emuCmd = isWin ? `emu.exe ${filenames.obj}` : `./emu ${filenames.obj}`;

    exec(asmCmd, (err) => {
        let assemblyLog = fs.existsSync(filenames.log) ? fs.readFileSync(filenames.log, 'utf8') : "";
        

        if (err || (assemblyLog && assemblyLog.includes("Errors found"))) {
            cleanup(filenames);
            return res.json({ error: true, message: assemblyLog || "Assembly Failed" });
        }

        exec(emuCmd, (emuErr, stdout, stderr) => {
            const listing = fs.existsSync(filenames.lst) ? fs.readFileSync(filenames.lst, 'utf8') : "";
            const traceData = fs.existsSync(filenames.trace) ? fs.readFileSync(filenames.trace, 'utf8') : "No trace generated.";
            
            res.json({ 
                error: false, 
                listing: listing,
                trace: traceData, 
                emulatorOutput: stdout || stderr || "Execution completed."
            });


            cleanup(filenames);
        });
    });
});

function cleanup(files) {
    Object.values(files).forEach(f => { if (fs.existsSync(f)) fs.unlinkSync(f); });
}


const PORT = process.env.PORT || 3000;
app.listen(PORT, () => console.log(`Backend online at port ${PORT}`));