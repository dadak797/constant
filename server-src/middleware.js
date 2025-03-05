import multer from "multer";


const storage = multer.diskStorage({
  destination: function (req, file, callback) {
    callback(null, `${process.env.UPLOAD_PATH}`);
  },
  filename: function (req, file, callback) {
    callback(null, file.originalname);
  }
});

export const upload = multer({ storage: storage });