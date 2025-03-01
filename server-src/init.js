import "dotenv/config";
import httpServer from "./server.js";
import fs from "fs";


const PORT = 4000;

const handleListening = () => {
  console.log(`Server listening on port http://localhost:${PORT} 🚀`);
};

// Initialize upload path
if (!fs.existsSync(`${process.env.UPLOAD_PATH}`)) {
  fs.mkdirSync(`${process.env.UPLOAD_PATH}`);
}

httpServer.listen(PORT, handleListening);