import path from "path";


const __dirname = path.resolve();

export const getHome = (req, res) => {
  return res.sendFile(__dirname + "/index.html");
};