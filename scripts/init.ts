import fs from "node:fs";
import { spawnSync, SpawnSyncOptions } from "node:child_process";

function run(command: string, args?: string[], options?: SpawnSyncOptions) {
  const result = spawnSync(command, args, { stdio: "inherit", ...options });
  if (result.error) {
    throw result.error;
  }
  if (result.status !== 0) {
    throw new Error(`Exited with status ${result.status}`);
  }
}

const REFERENCES_DIR = "references";
run("mkdir", ["-p", REFERENCES_DIR]);
if (!fs.existsSync(`${REFERENCES_DIR}/graphql-js`)) {
  run(
    "git",
    [
      "clone",
      "--depth=1",
      "--branch=v16.10.0",
      "https://github.com/graphql/graphql-js.git",
    ],
    {
      cwd: REFERENCES_DIR,
    }
  );
}
